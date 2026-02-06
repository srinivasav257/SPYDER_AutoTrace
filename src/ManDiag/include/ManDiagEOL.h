#pragma once
/**
 * @file ManDiagEOL.h
 * @brief Manufacturing Diagnostics End-of-Line (EOL) Command Types.
 *
 * EOL commands work with the UART serial interface. This module provides three
 * types of Manufacturing Diagnostics via UART (MDU) commands with increasing levels of validation:
 *
 * 1. MDU_Tx: Send command, don't care about response (fire and forget)
 * 2. MDU_TxRx: Send command, match entire response string with expected
 * 3. MDU_Type1: Send command, parse response, validate individual fields
 *              (status, data length, data bytes) with user-selectable validation
 *
 * All commands use the ManDiag protocol format:
 * Request:  6D643E <GroupId> <TestId> <Operation> <DataLength> [DataBytes...]
 * Response: 6D643C <GroupId> <TestId> <Operation> <Status> <DataLength> [DataBytes...]
 */

#include "ManDiagProtocol.h"
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <functional>

// Forward declaration
namespace SerialManager {
    class SerialPortManager;
}

namespace ManDiag {
namespace EOL {

//=============================================================================
// EOL Command Configuration
//=============================================================================

/**
 * @brief Configuration for EOL commands
 */
struct EOLConfig {
    QString portName;               ///< Serial port to use
    int timeoutMs = DEFAULT_TIMEOUT_MS;  ///< Command timeout
    int pendingWaitMs = DEFAULT_PENDING_WAIT_MS; ///< Max wait for pending responses
    int retries = DEFAULT_RETRIES;  ///< Number of retry attempts
    bool autoRetryOnPending = true; ///< Automatically retry on pending status (0xAA)
};

//=============================================================================
// EOL Command Result
//=============================================================================

/**
 * @brief Result of an EOL command execution
 */
struct EOLResult {
    bool success = false;           ///< Whether command succeeded
    QString message;                ///< Result or error message
    ManDiagResponse response;       ///< Parsed response (for MD_Type1)
    QString rawResponse;            ///< Raw response string
    int attempts = 0;               ///< Number of attempts made
    
    /**
     * @brief Convert to QVariantMap for command registry integration
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["success"] = success;
        map["message"] = message;
        map["rawResponse"] = rawResponse;
        map["attempts"] = attempts;
        
        if (response.valid) {
            map["parsedResponse"] = response.toVariantMap();
        }
        
        return map;
    }
    
    static EOLResult Success(const QString& msg = "OK", 
                             const ManDiagResponse& resp = ManDiagResponse()) {
        EOLResult r;
        r.success = true;
        r.message = msg;
        r.response = resp;
        r.rawResponse = resp.rawResponse;
        return r;
    }
    
    static EOLResult Failure(const QString& msg, const QString& raw = "") {
        EOLResult r;
        r.success = false;
        r.message = msg;
        r.rawResponse = raw;
        return r;
    }
};

//=============================================================================
// MDU_Tx Command (Transmit Only via UART)
//=============================================================================

/**
 * @brief MDU_Tx: Send command via UART without waiting for or validating response.
 *
 * Use this command type when you just want to send a command and don't
 * care about the response. Useful for:
 * - Initialization commands
 * - Configuration commands where response doesn't matter
 * - Broadcast commands
 *
 * @param command The hex command string (e.g., "6D643E 00 01 01 00 01 01")
 * @param config EOL configuration
 * @return Result indicating if send was successful
 */
EOLResult MD_Tx(const QString& command, const EOLConfig& config);

/**
 * @brief MDU_Tx with individual parameters (builds command internally)
 *
 * @param groupId Group ID (hex byte)
 * @param testId Test ID (hex byte)
 * @param operation Operation code (hex byte)
 * @param dataLength Data length (hex byte)
 * @param dataBytes Optional data bytes
 * @param config EOL configuration
 * @return Result indicating if send was successful
 */
EOLResult MD_Tx(const QString& groupId,
                const QString& testId,
                const QString& operation,
                const QString& dataLength,
                const QString& dataBytes,
                const EOLConfig& config);

//=============================================================================
// MDU_TxRx Command (Transmit and Match Response via UART)
//=============================================================================

/**
 * @brief MDU_TxRx: Send command via UART and match entire response with expected string.
 *
 * Use this command type when you want to verify the exact response string.
 * The response is matched as a whole (after normalizing whitespace).
 *
 * @param command The hex command string to send
 * @param expectedResponse The expected response string to match
 * @param config EOL configuration
 * @return Result indicating if response matched
 */
EOLResult MD_TxRx(const QString& command,
                  const QString& expectedResponse,
                  const EOLConfig& config);

/**
 * @brief MDU_TxRx with partial match option
 *
 * @param command The hex command string to send
 * @param expectedPattern Pattern to search for in response (contains match)
 * @param exactMatch If true, requires exact match; if false, checks if response contains pattern
 * @param config EOL configuration
 * @return Result indicating if response matched
 */
EOLResult MD_TxRx(const QString& command,
                  const QString& expectedPattern,
                  bool exactMatch,
                  const EOLConfig& config);

//=============================================================================
// MDU_Type1 Command (Transmit, Parse, and Validate via UART)
//=============================================================================

/**
 * @brief MDU_Type1: Send command via UART, parse response, and validate individual fields.
 *
 * This is the most flexible command type. It:
 * 1. Sends the command
 * 2. Waits for response with "6D643C" prefix
 * 3. Parses response into structured fields
 * 4. Validates selected fields against expected values
 *
 * Users can choose which fields to validate:
 * - Status byte only
 * - Status + Data Length
 * - Status + Data Length + Data Bytes
 *
 * @param command The hex command string to send
 * @param validation Validation options specifying what to check
 * @param config EOL configuration
 * @return Result with parsed response and validation status
 */
EOLResult MD_Type1(const QString& command,
                   const ValidationOptions& validation,
                   const EOLConfig& config);

/**
 * @brief MDU_Type1 with individual command parameters
 *
 * @param groupId Group ID (hex byte)
 * @param testId Test ID (hex byte)
 * @param operation Operation code (hex byte)
 * @param dataLength Data length (hex byte)
 * @param dataBytes Optional data bytes
 * @param validation Validation options
 * @param config EOL configuration
 * @return Result with parsed response and validation status
 */
EOLResult MD_Type1(const QString& groupId,
                   const QString& testId,
                   const QString& operation,
                   const QString& dataLength,
                   const QString& dataBytes,
                   const ValidationOptions& validation,
                   const EOLConfig& config);

/**
 * @brief MDU_Type1 with simple status validation only
 *
 * Convenience overload that validates only the status byte.
 *
 * @param command The hex command string to send
 * @param expectedStatus Expected status byte (default: "01" for success)
 * @param config EOL configuration
 * @return Result with parsed response and validation status
 */
EOLResult MD_Type1_StatusOnly(const QString& command,
                               const QString& expectedStatus,
                               const EOLConfig& config);

/**
 * @brief MDU_Type1 with status and data length validation
 *
 * @param command The hex command string to send
 * @param expectedStatus Expected status byte
 * @param expectedDataLength Expected data length byte
 * @param config EOL configuration
 * @return Result with parsed response and validation status
 */
EOLResult MD_Type1_StatusAndLength(const QString& command,
                                    const QString& expectedStatus,
                                    const QString& expectedDataLength,
                                    const EOLConfig& config);

//=============================================================================
// Helper: Send and Receive with Retry and Pending Handling
//=============================================================================

/**
 * @brief Internal function to send command and receive response with retry logic.
 *
 * Handles:
 * - Multiple retries on failure
 * - Automatic retry on pending status (0xAA)
 * - Timeout management
 *
 * @param command Command to send
 * @param config EOL configuration
 * @return Result with raw response
 */
EOLResult sendAndReceive(const QString& command, const EOLConfig& config);

//=============================================================================
// EOL Command Registry Integration
//=============================================================================

/**
 * @brief Register all EOL commands with the CommandRegistry.
 *
 * Call this at application startup to make EOL commands available
 * in the test editor UI.
 */
void registerEOLCommands();

} // namespace EOL
} // namespace ManDiag
