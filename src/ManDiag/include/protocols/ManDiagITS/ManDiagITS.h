#pragma once
/**
 * @file ManDiagITS.h
 * @brief ManDiag ITS protocol commands.
 */

#include "core/ManDiagProtocol.h"
#include <QVariantMap>

namespace ManDiag::ITS {

/**
 * @brief Runtime configuration for ITS command execution.
 */
struct ITSConfig {
    QString portName = "COM1";
    int timeoutMs = Protocol::DEFAULT_TIMEOUT_MS;
    int pendingTimeoutMs = Protocol::DEFAULT_PENDING_TIMEOUT_MS;
    int repetition = 1;
    int retryOnPendingDelayMs = 0;
};

/**
 * @brief Result object for ITS command execution.
 */
struct ITSResult {
    bool success = false;
    QString message;
    Protocol::Frame response;
    QString rawResponse;
    int attempts = 0;
    int repetitionsCompleted = 0;

    QVariantMap toVariantMap() const
    {
        QVariantMap map;
        map["success"] = success;
        map["message"] = message;
        map["raw_response"] = rawResponse;
        map["attempts"] = attempts;
        map["repetitions_completed"] = repetitionsCompleted;
        map["response"] = response.toVariantMap();
        return map;
    }

    static ITSResult Failure(const QString& msg,
                             const QString& raw = QString(),
                             const Protocol::Frame& frame = {})
    {
        ITSResult result;
        result.success = false;
        result.message = msg;
        result.rawResponse = raw;
        result.response = frame;
        return result;
    }

    static ITSResult Success(const QString& msg,
                             const Protocol::Frame& frame,
                             const QString& raw,
                             int attemptCount,
                             int completedRepetitions = 0)
    {
        ITSResult result;
        result.success = true;
        result.message = msg;
        result.response = frame;
        result.rawResponse = raw;
        result.attempts = attemptCount;
        result.repetitionsCompleted = completedRepetitions;
        return result;
    }
};

/**
 * @brief Command 1: request with fixed-response comparison.
 */
ITSResult MD_ITS_Request_Fixed_response(const QString& requestCommand,
                                        const QString& expectedResponse,
                                        const ITSConfig& config);

/**
 * @brief Command 2: request with variable-response field checks.
 *
 * `XX` means don't-care for each field:
 * - Expected Status Byte: "XX" skips status check
 * - Expected Data Length: "XX" skips data-length check
 * - Expected Data Bytes: token-level `XX` wildcard; field value "XX" skips byte check
 */
ITSResult MD_ITS_request_Variable_reponse(const QString& requestCommand,
                                          const QString& expectedStatusByte,
                                          const QString& expectedDataLength,
                                          const QString& expectedDataBytes,
                                          const ITSConfig& config);

/**
 * @brief Register ITS protocol commands in CommandRegistry.
 */
void registerITSCommands();

} // namespace ManDiag::ITS
