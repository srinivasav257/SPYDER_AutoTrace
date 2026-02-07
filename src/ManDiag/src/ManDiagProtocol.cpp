/**
 * @file ManDiagProtocol.cpp
 * @brief Implementation of ManDiag protocol parsing and utilities.
 */

#include "ManDiagProtocol.h"
#include <QDebug>
#include <QRegularExpression>

namespace ManDiag {

//=============================================================================
// Status Code Functions
//=============================================================================

QString statusCodeToString(StatusCode code)
{
    switch (code) {
        case StatusCode::Success:       return "Success";
        case StatusCode::GeneralError:  return "General Error";
        case StatusCode::InvalidCommand: return "Invalid Command";
        case StatusCode::InvalidParam:  return "Invalid Parameter";
        case StatusCode::NotSupported:  return "Not Supported";
        case StatusCode::Pending:       return "Pending";
        default:                        return "Unknown";
    }
}

StatusCode statusCodeFromHex(const QString& hex)
{
    bool ok;
    uint value = hex.toUInt(&ok, 16);
    if (!ok) return StatusCode::Unknown;
    
    switch (value) {
        case 0x01: return StatusCode::Success;
        case 0x02: return StatusCode::GeneralError;
        case 0x03: return StatusCode::InvalidCommand;
        case 0x04: return StatusCode::InvalidParam;
        case 0x05: return StatusCode::NotSupported;
        case 0xAA: return StatusCode::Pending;
        default:   return StatusCode::Unknown;
    }
}

//=============================================================================
// ManDiagResponse Methods
//=============================================================================

int ManDiagResponse::getDataLength() const
{
    bool ok;
    int len = dataLength.toInt(&ok, 16);
    return ok ? len : 0;
}

QByteArray ManDiagResponse::getDataAsBytes() const
{
    return hexToBytes(dataBytes);
}

QString ManDiagResponse::getDataAsAscii(int skipBytes) const
{
    return hexToAscii(dataBytes, skipBytes);
}

QVariantMap ManDiagResponse::toVariantMap() const
{
    QVariantMap map;
    map["valid"] = valid;
    map["rawResponse"] = rawResponse;
    map["prefix"] = prefix;
    map["groupId"] = groupId;
    map["testId"] = testId;
    map["operation"] = operation;
    map["status"] = status;
    map["statusText"] = statusCodeToString(statusCode());
    map["dataLength"] = dataLength;
    map["dataLengthInt"] = getDataLength();
    map["dataBytes"] = dataBytes;
    map["success"] = isSuccess();
    map["pending"] = isPending();
    return map;
}

//=============================================================================
// Protocol Parsing
//=============================================================================

ManDiagResponse parseResponse(const QString& response)
{
    ManDiagResponse result;
    result.rawResponse = response;
    
    // Split response by whitespace
    QStringList parts = response.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    
    // Minimum required fields: prefix, groupId, testId, operation, status, dataLength
    if (parts.size() < 6) {
        qWarning() << "ManDiag: Invalid response - not enough fields:" << response;
        return result;
    }
    
    // Validate prefix
    if (parts[0].toUpper() != QString(RESPONSE_PREFIX).toUpper()) {
        qWarning() << "ManDiag: Invalid prefix:" << parts[0] << "expected:" << RESPONSE_PREFIX;
        return result;
    }
    
    result.valid = true;
    result.prefix = parts[0].toUpper();
    result.groupId = parts[1].toUpper();
    result.testId = parts[2].toUpper();
    result.operation = parts[3].toUpper();
    result.status = parts[4].toUpper();
    result.dataLength = parts[5].toUpper();
    
    // Extract data bytes if present
    if (parts.size() > 6) {
        QStringList dataList;
        for (int i = 6; i < parts.size(); ++i) {
            dataList.append(parts[i].toUpper());
        }
        result.dataBytes = dataList.join(" ");
    }
    
    qDebug() << "ManDiag: Parsed response - Status:" << result.status
             << "DataLen:" << result.dataLength
             << "Data:" << result.dataBytes;
    
    return result;
}

QString buildRequest(const QString& groupId,
                     const QString& testId,
                     const QString& operation,
                     const QString& dataLength,
                     const QString& dataBytes)
{
    QStringList parts;
    parts << REQUEST_PREFIX;
    parts << groupId.toUpper();
    parts << testId.toUpper();
    parts << operation.toUpper();
    parts << dataLength.toUpper();
    
    if (!dataBytes.isEmpty()) {
        parts << dataBytes.toUpper();
    }
    
    return parts.join(" ");
}

QPair<bool, QString> validateResponse(const ManDiagResponse& response,
                                        const ValidationOptions& options)
{
    if (!response.valid) {
        return {false, "Invalid response format"};
    }
    
    // Validate status
    if (options.validateStatus) {
        if (response.status.toUpper() != options.expectedStatus.toUpper()) {
            return {false, QString("Status mismatch - Received: %1, Expected: %2")
                            .arg(response.status, options.expectedStatus)};
        }
    }
    
    // Validate data length
    if (options.validateDataLength) {
        if (response.dataLength.toUpper() != options.expectedDataLength.toUpper()) {
            return {false, QString("Data length mismatch - Received: %1, Expected: %2")
                            .arg(response.dataLength, options.expectedDataLength)};
        }
    }
    
    // Validate data bytes
    if (options.validateData && !options.expectedData.isEmpty()) {
        // Only validate if data length is not "00"
        if (response.dataLength != "00") {
            // Remove spaces for comparison
            QString receivedData = response.dataBytes.toUpper().remove(' ');
            QString expectedData = options.expectedData.toUpper().remove(' ');
            
            if (receivedData != expectedData) {
                return {false, QString("Data bytes mismatch - Received: %1, Expected: %2")
                                .arg(response.dataBytes, options.expectedData)};
            }
        }
    }
    
    return {true, "Response validated successfully"};
}

//=============================================================================
// Utility Functions (hexToBytes/bytesToHex are now inline in header via HexUtils)
//=============================================================================

QString hexToAscii(const QString& hex, int skipBytes)
{
    QString cleaned = hex;
    cleaned.remove(' ');
    
    if (cleaned.length() < skipBytes * 2) {
        return QString();
    }
    
    // Skip the specified number of bytes
    QString sliced = cleaned.mid(skipBytes * 2);
    
    QByteArray bytes = QByteArray::fromHex(sliced.toLatin1());
    return QString::fromLatin1(bytes);
}

QVector<uint8_t> extractBytes(const QString& hex)
{
    QVector<uint8_t> result;
    QStringList parts = hex.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    
    for (const QString& part : parts) {
        bool ok;
        uint8_t byte = part.toUInt(&ok, 16);
        if (ok) {
            result.append(byte);
        }
    }
    
    return result;
}

} // namespace ManDiag
