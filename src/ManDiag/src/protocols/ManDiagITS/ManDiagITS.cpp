/**
 * @file ManDiagITS.cpp
 * @brief Implementation of ManDiag ITS commands.
 */

#include "protocols/ManDiagITS/ManDiagITS.h"
#include "CommandRegistry.h"
#include <SerialManager.h>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

using namespace SerialManager;

namespace ManDiag::ITS {

namespace {

struct VariableExpectation {
    bool valid = false;
    QString error;
    QString statusByte;
    QString dataLengthByte;
    QStringList dataBytes;
};

ITSResult sendAndReceiveSerial(const QByteArray& requestBytes, const ITSConfig& config)
{
    auto& serial = SerialPortManager::instance();

    if (!serial.isPortOpen(config.portName)) {
        const SerialResult openResult = serial.openPort(config.portName);
        if (!openResult.success) {
            return ITSResult::Failure("Failed to open port: " + openResult.errorMessage);
        }
    }

    const QByteArray prefixBytes = Protocol::tokensToBytes(Protocol::defaultPrefixBytes());
    const int timeoutMs = config.timeoutMs > 0 ? config.timeoutMs : Protocol::DEFAULT_TIMEOUT_MS;
    const int pendingTimeoutMs = config.pendingTimeoutMs > 0
        ? config.pendingTimeoutMs
        : Protocol::DEFAULT_PENDING_TIMEOUT_MS;

    QElapsedTimer pendingTimer;
    pendingTimer.start();

    int attempts = 0;
    while (true) {
        ++attempts;
        serial.clearBuffers(config.portName);

        const SerialResult sendResult = serial.send(config.portName, requestBytes);
        if (!sendResult.success) {
            return ITSResult::Failure("Send failed: " + sendResult.errorMessage);
        }

        SerialResult readResult = serial.readUntil(config.portName, prefixBytes, timeoutMs);
        QByteArray buffer = readResult.data;
        if (!readResult.success && buffer.isEmpty()) {
            return ITSResult::Failure("No response: " + readResult.errorMessage);
        }

        QElapsedTimer readTimer;
        readTimer.start();
        Protocol::Frame frame = Protocol::extractFrame(buffer);

        // readUntil() returns as soon as prefix appears, so pull remaining bytes
        // until we can parse a complete frame or the response timeout expires.
        while (!frame.valid && readTimer.elapsed() < timeoutMs) {
            const int remaining = timeoutMs - static_cast<int>(readTimer.elapsed());
            if (remaining <= 0) {
                break;
            }

            const SerialResult extra = serial.read(config.portName, qMin(100, remaining));
            if (!extra.success || extra.data.isEmpty()) {
                break;
            }

            buffer.append(extra.data);
            frame = Protocol::extractFrame(buffer);
        }

        const QString rawHex = Protocol::normalizeTokens(Protocol::tokenizeHex(
            QString::fromLatin1(buffer.toHex().toUpper()), false), " ");

        if (!frame.valid) {
            ITSResult result = ITSResult::Failure(
                "Failed to parse ITS response: " + frame.error, rawHex, frame);
            result.attempts = attempts;
            return result;
        }

        if (frame.isPending() && config.retryOnPendingDelayMs > 0) {
            if (pendingTimer.elapsed() >= pendingTimeoutMs) {
                ITSResult result = ITSResult::Failure(
                    QString("Pending timeout exceeded (%1 ms)").arg(pendingTimeoutMs),
                    rawHex, frame);
                result.attempts = attempts;
                return result;
            }

            QThread::msleep(config.retryOnPendingDelayMs);
            continue;
        }

        ITSResult result = ITSResult::Success("Response received", frame, rawHex, attempts);
        return result;
    }
}

VariableExpectation parseVariableExpectation(const QString& expectedResponse)
{
    VariableExpectation expected;

    QString error;
    const QStringList tokens = Protocol::tokenizeHex(expectedResponse, true, &error);
    if (tokens.isEmpty()) {
        expected.error = error.isEmpty() ? "Expected response is empty" : error;
        return expected;
    }

    const QStringList prefix = Protocol::defaultPrefixBytes();
    const int prefLen = prefix.size();
    const bool hasFullFrame = tokens.size() >= (prefLen + 5) && tokens.mid(0, prefLen) == prefix;

    if (hasFullFrame) {
        expected.statusByte = tokens[prefLen + 3];
        expected.dataLengthByte = tokens[prefLen + 4];
        expected.dataBytes = tokens.mid(prefLen + 5);
        expected.valid = true;
        return expected;
    }

    if (tokens.size() >= 2) {
        expected.statusByte = tokens[0];
        expected.dataLengthByte = tokens[1];
        expected.dataBytes = tokens.mid(2);
        expected.valid = true;
        return expected;
    }

    expected.error = "Expected variable response must contain either full frame or: <Status> <Length> [Data...]";
    return expected;
}

bool isDontCareField(const QString& value)
{
    const QString normalized = value.trimmed().toUpper();
    return normalized.isEmpty() || normalized == "XX";
}

bool parseExpectedByte(const QString& fieldName, const QString& value, QString* parsed, QString* error)
{
    if (!parsed) {
        if (error) {
            *error = fieldName + ": internal parse error";
        }
        return false;
    }

    if (error) {
        error->clear();
    }

    QString localError;
    const QStringList tokens = Protocol::tokenizeHex(value, true, &localError);
    if (tokens.size() != 1) {
        if (error) {
            const QString reason = localError.isEmpty()
                ? QString("Expected single byte, got %1 token(s)").arg(tokens.size())
                : localError;
            *error = fieldName + ": " + reason;
        }
        return false;
    }

    *parsed = tokens[0];
    return true;
}

ITSConfig buildConfigFromContext(const QVariantMap& params, const QVariantMap& contextConfig)
{
    ITSConfig config;
    config.portName = contextConfig.value("default_serial_port", "COM1").toString();
    config.timeoutMs = contextConfig.value("mandiag_its_timeout_ms", Protocol::DEFAULT_TIMEOUT_MS).toInt();
    config.pendingTimeoutMs = contextConfig.value("mandiag_its_pending_timeout_ms",
                                                  Protocol::DEFAULT_PENDING_TIMEOUT_MS).toInt();
    config.repetition = qMax(1, params.value("repeatation", 1).toInt());
    config.retryOnPendingDelayMs = qMax(0, params.value("retry_on_pending_with_delay_ms", 0).toInt());
    return config;
}

TestExecutor::CommandResult toCommandResult(const ITSResult& result)
{
    if (result.success) {
        return TestExecutor::CommandResult::Success(result.message, result.toVariantMap());
    }

    TestExecutor::CommandResult failed = TestExecutor::CommandResult::Failure(result.message);
    failed.responseData = result.toVariantMap();
    return failed;
}

} // namespace

ITSResult MD_ITS_Request_Fixed_response(const QString& requestCommand,
                                        const QString& expectedResponse,
                                        const ITSConfig& config)
{
    QString error;
    const QStringList requestTokens = Protocol::tokenizeHex(requestCommand, false, &error);
    if (requestTokens.isEmpty()) {
        return ITSResult::Failure("Invalid request command: " + error);
    }

    const QStringList expectedTokens = Protocol::tokenizeHex(expectedResponse, true, &error);
    if (expectedTokens.isEmpty()) {
        return ITSResult::Failure("Invalid expected response: " + error);
    }

    const QByteArray requestBytes = Protocol::tokensToBytes(requestTokens);
    const int totalRepetitions = qMax(1, config.repetition);

    int totalAttempts = 0;
    ITSResult lastResult;
    for (int i = 0; i < totalRepetitions; ++i) {
        ITSResult cycle = sendAndReceiveSerial(requestBytes, config);
        totalAttempts += cycle.attempts;
        cycle.attempts = totalAttempts;
        cycle.repetitionsCompleted = i + 1;

        if (!cycle.success) {
            return cycle;
        }

        QString mismatch;
        if (!Protocol::bytesMatchWithWildcards(
                cycle.response.bytes, expectedTokens, &mismatch, true)) {
            ITSResult failed = ITSResult::Failure(
                QString("Fixed response mismatch on repetition %1: %2")
                    .arg(i + 1)
                    .arg(mismatch),
                cycle.rawResponse,
                cycle.response);
            failed.attempts = totalAttempts;
            failed.repetitionsCompleted = i + 1;
            return failed;
        }

        lastResult = cycle;
    }

    lastResult.success = true;
    lastResult.message = QString("Fixed response matched for %1 repetition(s)").arg(totalRepetitions);
    lastResult.attempts = totalAttempts;
    lastResult.repetitionsCompleted = totalRepetitions;
    return lastResult;
}

ITSResult MD_ITS_request_Variable_reponse(const QString& requestCommand,
                                          const QString& expectedStatusByte,
                                          const QString& expectedDataLength,
                                          const QString& expectedDataBytes,
                                          const ITSConfig& config)
{
    QString error;
    const QStringList requestTokens = Protocol::tokenizeHex(requestCommand, false, &error);
    if (requestTokens.isEmpty()) {
        return ITSResult::Failure("Invalid request command: " + error);
    }

    const bool skipStatusCheck = isDontCareField(expectedStatusByte);
    const bool skipDataLengthCheck = isDontCareField(expectedDataLength);
    const bool skipDataBytesCheck = isDontCareField(expectedDataBytes);

    QString parsedStatusByte;
    QString parsedDataLengthByte;
    QStringList parsedDataBytes;

    if (!skipStatusCheck && !parseExpectedByte("Expected Status Byte", expectedStatusByte, &parsedStatusByte, &error)) {
        return ITSResult::Failure("Invalid expected status byte: " + error);
    }

    if (!skipDataLengthCheck
        && !parseExpectedByte("Expected Data Length", expectedDataLength, &parsedDataLengthByte, &error)) {
        return ITSResult::Failure("Invalid expected data length: " + error);
    }

    if (!skipDataBytesCheck) {
        parsedDataBytes = Protocol::tokenizeHex(expectedDataBytes, true, &error);
        if (parsedDataBytes.isEmpty()) {
            return ITSResult::Failure("Invalid expected data bytes: " + error);
        }
    }

    ITSResult result = sendAndReceiveSerial(Protocol::tokensToBytes(requestTokens), config);
    if (!result.success) {
        return result;
    }

    if (!skipStatusCheck
        && !Protocol::tokenMatches(result.response.statusByte, parsedStatusByte)) {
        ITSResult failed = ITSResult::Failure(
            QString("Status byte mismatch. Expected %1, got %2")
                .arg(parsedStatusByte, result.response.statusByte),
            result.rawResponse,
            result.response);
        failed.attempts = result.attempts;
        return failed;
    }

    if (!skipDataLengthCheck
        && !Protocol::tokenMatches(result.response.dataLengthByte, parsedDataLengthByte)) {
        ITSResult failed = ITSResult::Failure(
            QString("Data length mismatch. Expected %1, got %2")
                .arg(parsedDataLengthByte, result.response.dataLengthByte),
            result.rawResponse,
            result.response);
        failed.attempts = result.attempts;
        return failed;
    }

    if (!skipDataBytesCheck) {
        QString mismatch;
        if (!Protocol::bytesMatchWithWildcards(
                result.response.dataBytes, parsedDataBytes, &mismatch, true)) {
            ITSResult failed = ITSResult::Failure(
                "Data bytes mismatch: " + mismatch,
                result.rawResponse,
                result.response);
            failed.attempts = result.attempts;
            return failed;
        }
    }

    result.success = true;
    result.message = "Variable response validation passed";
    result.repetitionsCompleted = 1;
    return result;
}

void registerITSCommands()
{
    using namespace TestExecutor;
    auto& registry = CommandRegistry::instance();

    registry.registerCommand({
        .id = "mandiag_its_fixed_response",
        .name = "MD_ITS_Request_Fixed_response",
        .description = "Send ITS request and compare full response with expected value.",
        .category = CommandCategory::ManDiagITS,
        .parameters = {
            {
                .name = "request_command",
                .displayName = "Request command",
                .description = "ITS request frame in hex (e.g. '6D643E 00 01 01 00 01 01')",
                .type = ParameterType::HexString,
                .defaultValue = "6D643E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "expected_response",
                .displayName = "Expected response",
                .description = "Expected full ITS response (supports XX wildcards)",
                .type = ParameterType::HexString,
                .defaultValue = "6D643E 00 01 01 01 00",
                .required = true
            },
            {
                .name = "repeatation",
                .displayName = "Repeatation",
                .description = "Number of repetitions to execute and validate",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = false,
                .minValue = 1,
                .maxValue = 100
            },
            {
                .name = "retry_on_pending_with_delay_ms",
                .displayName = "Retry on Pending with Delay",
                .description = "Delay in ms before retry when status is pending (AA). 0 disables retry.",
                .type = ParameterType::Duration,
                .defaultValue = 0,
                .required = false,
                .minValue = 0,
                .maxValue = 10000,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params,
                      const QVariantMap& config,
                      const std::atomic<bool>* /*cancel*/) -> CommandResult {
            const ITSConfig itsConfig = buildConfigFromContext(params, config);
            const QString request = params.value("request_command").toString();
            const QString expected = params.value("expected_response").toString();
            return toCommandResult(MD_ITS_Request_Fixed_response(request, expected, itsConfig));
        }
    });

    registry.registerCommand({
        .id = "mandiag_its_variable_response",
        .name = "MD_ITS_request_Variable_reponse",
        .description = "Send ITS request and validate status, data length, and data bytes. Use XX for don't-care bytes.",
        .category = CommandCategory::ManDiagITS,
        .parameters = {
            {
                .name = "request_command",
                .displayName = "Request command",
                .description = "ITS request frame in hex",
                .type = ParameterType::HexString,
                .defaultValue = "6D643E 50 04 00 00 01 01",
                .required = true
            },
            {
                .name = "expected_status_byte",
                .displayName = "Expected Status Byte",
                .description = "Expected status byte (e.g. '01'). Use XX to skip status check.",
                .type = ParameterType::HexString,
                .defaultValue = "01",
                .required = false
            },
            {
                .name = "expected_data_length",
                .displayName = "Expected Data Length",
                .description = "Expected data length byte (e.g. '02'). Use XX to skip data length check.",
                .type = ParameterType::HexString,
                .defaultValue = "02",
                .required = false
            },
            {
                .name = "expected_data_bytes",
                .displayName = "Expected Data Bytes",
                .description = "Expected data bytes (e.g. '01 XX'). Use XX as byte wildcard, or XX to skip this check.",
                .type = ParameterType::HexString,
                .defaultValue = "01 XX",
                .required = false
            }
        },
        .handler = [](const QVariantMap& params,
                      const QVariantMap& config,
                      const std::atomic<bool>* /*cancel*/) -> CommandResult {
            ITSConfig itsConfig = buildConfigFromContext(params, config);
            itsConfig.repetition = 1;
            const QString request = params.value("request_command").toString();
            QString expectedStatusByte = params.value("expected_status_byte").toString();
            QString expectedDataLength = params.value("expected_data_length").toString();
            QString expectedDataBytes = params.value("expected_data_bytes").toString();

            // Backward compatibility for older steps that still provide expected_response.
            if (expectedStatusByte.trimmed().isEmpty()
                && expectedDataLength.trimmed().isEmpty()
                && expectedDataBytes.trimmed().isEmpty()) {
                const QString legacyExpected = params.value("expected_response").toString();
                if (!legacyExpected.trimmed().isEmpty()) {
                    const VariableExpectation legacy = parseVariableExpectation(legacyExpected);
                    if (!legacy.valid) {
                        return CommandResult::Failure("Invalid legacy expected_response: " + legacy.error);
                    }
                    expectedStatusByte = legacy.statusByte;
                    expectedDataLength = legacy.dataLengthByte;
                    expectedDataBytes = legacy.dataBytes.join(" ");
                }
            }

            return toCommandResult(MD_ITS_request_Variable_reponse(
                request,
                expectedStatusByte,
                expectedDataLength,
                expectedDataBytes,
                itsConfig));
        }
    });

    qDebug() << "ManDiag ITS commands registered";
}

} // namespace ManDiag::ITS
