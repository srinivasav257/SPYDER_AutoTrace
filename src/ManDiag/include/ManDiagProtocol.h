#pragma once
/**
 * @file ManDiagProtocol.h
 * @brief Manufacturing Diagnostics Protocol definitions and utilities.
 *
 * This file defines the ManDiag protocol structure used for communication
 * with the infotainment unit. It supports two categories:
 * - EOL (End of Line): Serial interface communication
 * - MOL (Mid of Line): CAN interface communication
 *
 * ManDiag Frame Format:
 * Request:  6D643E <GroupId> <TestId> <Operation> <DataLength> [DataBytes...]
 * Response: 6D643C <GroupId> <TestId> <Operation> <Status> <DataLength> [DataBytes...]
 *
 * The prefix "6D643E" is the request marker and "6D643C" is the response marker.
 */

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <QVector>

namespace ManDiag {

//=============================================================================
// Protocol Constants
//=============================================================================

/// Request prefix "md>" in hex
constexpr const char* REQUEST_PREFIX = "6D643E";

/// Response prefix "md<" in hex
constexpr const char* RESPONSE_PREFIX = "6D643C";

/// Default successful status
constexpr const char* STATUS_SUCCESS = "01";

/// Pending status - retry needed
constexpr const char* STATUS_PENDING = "AA";

/// Default timeout in milliseconds
constexpr int DEFAULT_TIMEOUT_MS = 5000;

/// Default pending wait time in milliseconds
constexpr int DEFAULT_PENDING_WAIT_MS = 15000;

/// Default retry count
constexpr int DEFAULT_RETRIES = 1;

//=============================================================================
// Status Codes
//=============================================================================

/**
 * @brief ManDiag response status codes
 */
enum class StatusCode : uint8_t {
    Success         = 0x01,     ///< Command executed successfully
    GeneralError    = 0x02,     ///< General error
    InvalidCommand  = 0x03,     ///< Invalid command
    InvalidParam    = 0x04,     ///< Invalid parameter
    NotSupported    = 0x05,     ///< Command not supported
    Pending         = 0xAA,     ///< Response pending, retry later
    Unknown         = 0xFF      ///< Unknown status
};

/**
 * @brief Convert status code to string
 */
QString statusCodeToString(StatusCode code);

/**
 * @brief Convert hex string to StatusCode
 */
StatusCode statusCodeFromHex(const QString& hex);

//=============================================================================
// Validation Options
//=============================================================================

/**
 * @brief Options for what to validate in a Type1 command response
 */
struct ValidationOptions {
    bool validateStatus = true;         ///< Validate the status byte
    bool validateDataLength = false;    ///< Validate the data length byte
    bool validateData = false;          ///< Validate the data bytes
    
    QString expectedStatus = STATUS_SUCCESS;
    QString expectedDataLength = "00";
    QString expectedData;
    
    /**
     * @brief Create options to validate all fields
     */
    static ValidationOptions all(const QString& status = STATUS_SUCCESS,
                                  const QString& dataLength = "00",
                                  const QString& data = "") {
        return {true, true, !data.isEmpty(), status, dataLength, data};
    }
    
    /**
     * @brief Create options to validate only status
     */
    static ValidationOptions statusOnly(const QString& status = STATUS_SUCCESS) {
        return {true, false, false, status, "00", ""};
    }
    
    /**
     * @brief Create options to validate status and data length
     */
    static ValidationOptions statusAndLength(const QString& status = STATUS_SUCCESS,
                                              const QString& dataLength = "00") {
        return {true, true, false, status, dataLength, ""};
    }
};

//=============================================================================
// ManDiag Response
//=============================================================================

/**
 * @brief Parsed ManDiag response structure
 */
struct ManDiagResponse {
    bool valid = false;             ///< Whether response was successfully parsed
    QString rawResponse;            ///< Raw response string
    
    // Parsed fields (all hex strings)
    QString prefix;                 ///< Response prefix (should be "6D643C")
    QString groupId;                ///< Group ID
    QString testId;                 ///< Test ID
    QString operation;              ///< Operation code
    QString status;                 ///< Status byte
    QString dataLength;             ///< Data length byte
    QString dataBytes;              ///< Data bytes (space-separated hex)
    
    /**
     * @brief Get status as enum
     */
    StatusCode statusCode() const { return statusCodeFromHex(status); }
    
    /**
     * @brief Check if response indicates success
     */
    bool isSuccess() const { return status == STATUS_SUCCESS; }
    
    /**
     * @brief Check if response indicates pending (retry needed)
     */
    bool isPending() const { return status == STATUS_PENDING; }
    
    /**
     * @brief Get data length as integer
     */
    int getDataLength() const;
    
    /**
     * @brief Get data bytes as QByteArray
     */
    QByteArray getDataAsBytes() const;
    
    /**
     * @brief Get data bytes as ASCII string (skipping first n bytes)
     * @param skipBytes Number of bytes to skip from the beginning
     */
    QString getDataAsAscii(int skipBytes = 0) const;
    
    /**
     * @brief Convert response to QVariantMap for command results
     */
    QVariantMap toVariantMap() const;
    
    /**
     * @brief Create an invalid/empty response
     */
    static ManDiagResponse invalid(const QString& raw = "") {
        ManDiagResponse r;
        r.valid = false;
        r.rawResponse = raw;
        return r;
    }
};

//=============================================================================
// ManDiag Protocol Parser
//=============================================================================

/**
 * @brief Parse a ManDiag response string
 *
 * Expected format (space-separated hex bytes):
 * "6D643C <GroupId> <TestId> <Operation> <Status> <DataLength> [DataBytes...]"
 *
 * @param response Raw response string (hex bytes, space-separated)
 * @return Parsed response structure
 */
ManDiagResponse parseResponse(const QString& response);

/**
 * @brief Build a ManDiag request command string
 *
 * @param groupId Group ID (hex byte)
 * @param testId Test ID (hex byte)
 * @param operation Operation code (hex byte)
 * @param dataLength Data length (hex byte)
 * @param dataBytes Optional data bytes (space-separated hex)
 * @return Complete request string
 */
QString buildRequest(const QString& groupId,
                     const QString& testId,
                     const QString& operation,
                     const QString& dataLength,
                     const QString& dataBytes = "");

/**
 * @brief Validate a ManDiag response against expected values
 *
 * @param response Parsed response
 * @param options Validation options specifying what to check
 * @return QPair<bool, QString> - success flag and error message if failed
 */
QPair<bool, QString> validateResponse(const ManDiagResponse& response,
                                        const ValidationOptions& options);

//=============================================================================
// Utility Functions
//=============================================================================

/**
 * @brief Convert hex string to byte array
 * @param hex Hex string (with or without spaces/separators)
 */
QByteArray hexToBytes(const QString& hex);

/**
 * @brief Convert byte array to hex string
 * @param bytes Raw bytes
 * @param separator Separator between bytes (default: space)
 */
QString bytesToHex(const QByteArray& bytes, const QString& separator = " ");

/**
 * @brief Convert hex string to ASCII, skipping first n bytes
 * @param hex Hex string (space-separated)
 * @param skipBytes Number of bytes to skip
 */
QString hexToAscii(const QString& hex, int skipBytes = 0);

/**
 * @brief Extract individual bytes from space-separated hex string
 * @param hex Space-separated hex string
 * @return Vector of individual byte values
 */
QVector<uint8_t> extractBytes(const QString& hex);

} // namespace ManDiag
