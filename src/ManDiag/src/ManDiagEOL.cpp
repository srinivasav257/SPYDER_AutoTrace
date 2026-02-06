/**
 * @file ManDiagEOL.cpp
 * @brief Implementation of EOL (End-of-Line) ManDiag commands.
 */

#include "ManDiagEOL.h"
#include "ManDiagProtocol.h"
#include "CommandRegistry.h"
#include <SerialManager.h>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <QCoreApplication>

using namespace SerialManager;

namespace ManDiag {
namespace EOL {

//=============================================================================
// Internal Helper: Send and Receive with Retry
//=============================================================================

EOLResult sendAndReceive(const QString& command, const EOLConfig& config)
{
    auto& serial = SerialPortManager::instance();
    
    // Ensure port is open
    if (!serial.isPortOpen(config.portName)) {
        auto openResult = serial.openPort(config.portName);
        if (!openResult.success) {
            return EOLResult::Failure("Failed to open port: " + openResult.errorMessage);
        }
    }
    
    // Clear buffers before sending
    serial.clearBuffers(config.portName);
    
    QElapsedTimer pendingTimer;
    pendingTimer.start();
    
    EOLResult result;
    result.attempts = 0;
    
    // Retry loop
    for (int retry = 0; retry < config.retries; ++retry) {
        result.attempts++;
        
        // Send command
        QByteArray cmdBytes = hexToBytes(command);
        auto sendResult = serial.send(config.portName, cmdBytes);
        
        if (!sendResult.success) {
            qWarning() << "ManDiag EOL: Send failed on attempt" << (retry + 1) 
                       << ":" << sendResult.errorMessage;
            continue;
        }
        
        qDebug() << "ManDiag EOL: Sent command:" << command;
        
        // Wait for response with prefix pattern
        QByteArray prefixBytes = hexToBytes(RESPONSE_PREFIX);
        auto readResult = serial.readUntil(config.portName, prefixBytes, config.timeoutMs);
        
        if (!readResult.success || readResult.data.isEmpty()) {
            qWarning() << "ManDiag EOL: No response on attempt" << (retry + 1);
            continue;
        }
        
        // Convert received data to hex string
        QString rawResponse = bytesToHex(readResult.data);
        result.rawResponse = rawResponse;
        
        qDebug() << "ManDiag EOL: Received response:" << rawResponse;
        
        // Parse the response
        ManDiagResponse parsedResponse = parseResponse(rawResponse);
        result.response = parsedResponse;
        
        if (!parsedResponse.valid) {
            qWarning() << "ManDiag EOL: Failed to parse response on attempt" << (retry + 1);
            continue;
        }
        
        // Check for pending status
        if (parsedResponse.isPending() && config.autoRetryOnPending) {
            qDebug() << "ManDiag EOL: Pending status received, waiting...";
            
            // Check if we've exceeded pending wait time
            if (pendingTimer.elapsed() >= config.pendingWaitMs) {
                qWarning() << "ManDiag EOL: Pending timeout exceeded";
                result.success = false;
                result.message = "Pending timeout exceeded";
                return result;
            }
            
            // Wait before retry (500ms intervals)
            for (int i = 0; i < 10; ++i) {
                QThread::msleep(500);
                QCoreApplication::processEvents();
                
                if (pendingTimer.elapsed() >= config.pendingWaitMs) {
                    break;
                }
            }
            
            // Reset retry counter to allow continued retries on pending
            retry = -1;  // Will be incremented to 0
            continue;
        }
        
        // Success - got a valid non-pending response
        result.success = true;
        result.message = "Response received successfully";
        return result;
    }
    
    // All retries exhausted
    result.success = false;
    if (result.message.isEmpty()) {
        result.message = QString("Failed after %1 attempts").arg(result.attempts);
    }
    return result;
}

//=============================================================================
// MD_Tx Implementation (Transmit Only)
//=============================================================================

EOLResult MD_Tx(const QString& command, const EOLConfig& config)
{
    auto& serial = SerialPortManager::instance();
    
    // Ensure port is open
    if (!serial.isPortOpen(config.portName)) {
        auto openResult = serial.openPort(config.portName);
        if (!openResult.success) {
            return EOLResult::Failure("Failed to open port: " + openResult.errorMessage);
        }
    }
    
    // Send command
    QByteArray cmdBytes = hexToBytes(command);
    auto sendResult = serial.send(config.portName, cmdBytes);
    
    if (!sendResult.success) {
        return EOLResult::Failure("Send failed: " + sendResult.errorMessage);
    }
    
    qDebug() << "ManDiag MD_Tx: Sent command:" << command;
    
    EOLResult result;
    result.success = true;
    result.message = "Command sent successfully";
    result.attempts = 1;
    return result;
}

EOLResult MD_Tx(const QString& groupId,
                const QString& testId,
                const QString& operation,
                const QString& dataLength,
                const QString& dataBytes,
                const EOLConfig& config)
{
    QString command = buildRequest(groupId, testId, operation, dataLength, dataBytes);
    return MD_Tx(command, config);
}

//=============================================================================
// MD_TxRx Implementation (Transmit and Match Response)
//=============================================================================

EOLResult MD_TxRx(const QString& command,
                  const QString& expectedResponse,
                  const EOLConfig& config)
{
    return MD_TxRx(command, expectedResponse, true, config);
}

EOLResult MD_TxRx(const QString& command,
                  const QString& expectedPattern,
                  bool exactMatch,
                  const EOLConfig& config)
{
    // Send and receive
    EOLResult result = sendAndReceive(command, config);
    
    if (!result.success) {
        return result;
    }
    
    // Normalize strings for comparison (remove extra whitespace, uppercase)
    QString normalizedResponse = result.rawResponse.toUpper().simplified();
    QString normalizedExpected = expectedPattern.toUpper().simplified();
    
    bool matched = false;
    if (exactMatch) {
        matched = (normalizedResponse == normalizedExpected);
    } else {
        matched = normalizedResponse.contains(normalizedExpected);
    }
    
    if (matched) {
        result.success = true;
        result.message = "Response matched expected pattern";
    } else {
        result.success = false;
        result.message = QString("Response mismatch - Expected: %1, Received: %2")
                            .arg(expectedPattern, result.rawResponse);
    }
    
    return result;
}

//=============================================================================
// MD_Type1 Implementation (Parse and Validate)
//=============================================================================

EOLResult MD_Type1(const QString& command,
                   const ValidationOptions& validation,
                   const EOLConfig& config)
{
    // Send and receive
    EOLResult result = sendAndReceive(command, config);
    
    if (!result.success) {
        return result;
    }
    
    // Validate response
    auto [valid, message] = validateResponse(result.response, validation);
    
    result.success = valid;
    result.message = message;
    
    return result;
}

EOLResult MD_Type1(const QString& groupId,
                   const QString& testId,
                   const QString& operation,
                   const QString& dataLength,
                   const QString& dataBytes,
                   const ValidationOptions& validation,
                   const EOLConfig& config)
{
    QString command = buildRequest(groupId, testId, operation, dataLength, dataBytes);
    return MD_Type1(command, validation, config);
}

EOLResult MD_Type1_StatusOnly(const QString& command,
                               const QString& expectedStatus,
                               const EOLConfig& config)
{
    ValidationOptions opts = ValidationOptions::statusOnly(expectedStatus);
    return MD_Type1(command, opts, config);
}

EOLResult MD_Type1_StatusAndLength(const QString& command,
                                    const QString& expectedStatus,
                                    const QString& expectedDataLength,
                                    const EOLConfig& config)
{
    ValidationOptions opts = ValidationOptions::statusAndLength(expectedStatus, expectedDataLength);
    return MD_Type1(command, opts, config);
}

//=============================================================================
// Command Registry Integration
//=============================================================================

void registerEOLCommands()
{
    using namespace TestExecutor;
    auto& registry = CommandRegistry::instance();
    
    // =========================================================================
    // MD_Tx - Transmit Only (Fire and Forget)
    // =========================================================================
    registry.registerCommand({
        .id = "mandiag_eol_tx",
        .name = "MD_Tx (Send Only)",
        .description = "Send ManDiag command without waiting for response. "
                       "Use for initialization or configuration commands where response doesn't matter.",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "hex_command",
                .displayName = "Command (Hex)",
                .description = "Complete hex command string (e.g., '6D 64 3E 00 01 01 00 01 01'). "
                               "Prefix 6D643E is auto-prepended if not present.",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "port",
                .displayName = "Serial Port",
                .description = "Serial port to use for communication",
                .type = ParameterType::ComPort,
                .defaultValue = "COM1",
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& config) -> CommandResult {
            QString hexCmd = params.value("hex_command").toString();
            QString port = params.value("port", config.value("default_serial_port", "COM1")).toString();
            
            EOLConfig eolConfig;
            eolConfig.portName = port;
            
            // Ensure prefix is present
            QString cleanCmd = hexCmd.toUpper().remove(' ');
            if (!cleanCmd.startsWith(REQUEST_PREFIX)) {
                hexCmd = QString(REQUEST_PREFIX) + " " + hexCmd;
            }
            
            EOLResult result = MD_Tx(hexCmd, eolConfig);
            
            if (result.success) {
                return CommandResult::Success(result.message, result.toVariantMap());
            } else {
                return CommandResult::Failure(result.message);
            }
        }
    });
    
    // =========================================================================
    // MD_TxRx - Transmit and Match Response
    // =========================================================================
    registry.registerCommand({
        .id = "mandiag_eol_txrx",
        .name = "MD_TxRx (Send & Match)",
        .description = "Send ManDiag command and match entire response string with expected value.",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "hex_command",
                .displayName = "Command (Hex)",
                .description = "Complete hex command string to send",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "expected_response",
                .displayName = "Expected Response",
                .description = "Expected hex response string to match",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3C 00 01 01 01 00",
                .required = true
            },
            {
                .name = "exact_match",
                .displayName = "Exact Match",
                .description = "If true, requires exact match; if false, checks if response contains pattern",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            },
            {
                .name = "port",
                .displayName = "Serial Port",
                .description = "Serial port to use",
                .type = ParameterType::ComPort,
                .defaultValue = "COM1",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = DEFAULT_TIMEOUT_MS,
                .required = false,
                .minValue = 100,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "retries",
                .displayName = "Retries",
                .description = "Number of retry attempts",
                .type = ParameterType::Integer,
                .defaultValue = DEFAULT_RETRIES,
                .required = false,
                .minValue = 1,
                .maxValue = 10
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& config) -> CommandResult {
            QString hexCmd = params.value("hex_command").toString();
            QString expected = params.value("expected_response").toString();
            bool exactMatch = params.value("exact_match", false).toBool();
            QString port = params.value("port", config.value("default_serial_port", "COM1")).toString();
            int timeoutMs = params.value("timeout_ms", DEFAULT_TIMEOUT_MS).toInt();
            int retries = params.value("retries", DEFAULT_RETRIES).toInt();
            
            EOLConfig eolConfig;
            eolConfig.portName = port;
            eolConfig.timeoutMs = timeoutMs;
            eolConfig.retries = retries;
            
            // Ensure prefix is present
            QString cleanCmd = hexCmd.toUpper().remove(' ');
            if (!cleanCmd.startsWith(REQUEST_PREFIX)) {
                hexCmd = QString(REQUEST_PREFIX) + " " + hexCmd;
            }
            
            EOLResult result = MD_TxRx(hexCmd, expected, exactMatch, eolConfig);
            
            if (result.success) {
                return CommandResult::Success(result.message, result.toVariantMap());
            } else {
                return CommandResult::Failure(result.message);
            }
        }
    });
    
    // =========================================================================
    // MD_Type1 - Parse and Validate Individual Fields
    // =========================================================================
    registry.registerCommand({
        .id = "mandiag_eol_type1",
        .name = "MD_Type1 (Parse & Validate)",
        .description = "Send ManDiag command, parse response, and validate individual fields "
                       "(status, data length, data bytes). Select which fields to validate.",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "hex_command",
                .displayName = "Command (Hex)",
                .description = "Complete hex command string to send",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "validate_status",
                .displayName = "Validate Status",
                .description = "Check if status byte matches expected value",
                .type = ParameterType::Boolean,
                .defaultValue = true,
                .required = false
            },
            {
                .name = "expected_status",
                .displayName = "Expected Status",
                .description = "Expected status byte (hex, e.g., '01' for success)",
                .type = ParameterType::HexString,
                .defaultValue = STATUS_SUCCESS,
                .required = false
            },
            {
                .name = "validate_data_length",
                .displayName = "Validate Data Length",
                .description = "Check if data length matches expected value",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            },
            {
                .name = "expected_data_length",
                .displayName = "Expected Data Length",
                .description = "Expected data length byte (hex, e.g., '00')",
                .type = ParameterType::HexString,
                .defaultValue = "00",
                .required = false
            },
            {
                .name = "validate_data",
                .displayName = "Validate Data",
                .description = "Check if data bytes match expected value",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            },
            {
                .name = "expected_data",
                .displayName = "Expected Data",
                .description = "Expected data bytes (hex, space-separated)",
                .type = ParameterType::HexString,
                .defaultValue = "",
                .required = false
            },
            {
                .name = "port",
                .displayName = "Serial Port",
                .description = "Serial port to use",
                .type = ParameterType::ComPort,
                .defaultValue = "COM1",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = DEFAULT_TIMEOUT_MS,
                .required = false,
                .minValue = 100,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "pending_wait_ms",
                .displayName = "Pending Wait",
                .description = "Maximum time to wait for pending (0xAA) responses",
                .type = ParameterType::Duration,
                .defaultValue = DEFAULT_PENDING_WAIT_MS,
                .required = false,
                .minValue = 1000,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "retries",
                .displayName = "Retries",
                .description = "Number of retry attempts",
                .type = ParameterType::Integer,
                .defaultValue = DEFAULT_RETRIES,
                .required = false,
                .minValue = 1,
                .maxValue = 10
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& config) -> CommandResult {
            QString hexCmd = params.value("hex_command").toString();
            QString port = params.value("port", config.value("default_serial_port", "COM1")).toString();
            int timeoutMs = params.value("timeout_ms", DEFAULT_TIMEOUT_MS).toInt();
            int pendingWaitMs = params.value("pending_wait_ms", DEFAULT_PENDING_WAIT_MS).toInt();
            int retries = params.value("retries", DEFAULT_RETRIES).toInt();
            
            // Build validation options
            ValidationOptions opts;
            opts.validateStatus = params.value("validate_status", true).toBool();
            opts.expectedStatus = params.value("expected_status", STATUS_SUCCESS).toString();
            opts.validateDataLength = params.value("validate_data_length", false).toBool();
            opts.expectedDataLength = params.value("expected_data_length", "00").toString();
            opts.validateData = params.value("validate_data", false).toBool();
            opts.expectedData = params.value("expected_data", "").toString();
            
            EOLConfig eolConfig;
            eolConfig.portName = port;
            eolConfig.timeoutMs = timeoutMs;
            eolConfig.pendingWaitMs = pendingWaitMs;
            eolConfig.retries = retries;
            
            // Ensure prefix is present
            QString cleanCmd = hexCmd.toUpper().remove(' ');
            if (!cleanCmd.startsWith(REQUEST_PREFIX)) {
                hexCmd = QString(REQUEST_PREFIX) + " " + hexCmd;
            }
            
            EOLResult result = MD_Type1(hexCmd, opts, eolConfig);
            
            if (result.success) {
                return CommandResult::Success(result.message, result.toVariantMap());
            } else {
                return CommandResult::Failure(result.message);
            }
        }
    });
    
    qDebug() << "ManDiag EOL commands registered";
}

} // namespace EOL
} // namespace ManDiag
