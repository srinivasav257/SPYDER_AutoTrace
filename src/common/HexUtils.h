#pragma once
/**
 * @file HexUtils.h
 * @brief Shared hex string ↔ byte array conversion utilities.
 *
 * Consolidates the duplicated hex helpers that were previously in both
 * CommandRegistry.h (TestExecutor) and ManDiagProtocol.h (ManDiag).
 */

#include <QByteArray>
#include <QString>

namespace HexUtils {

/**
 * @brief Convert hex string to byte array.
 * @param hex String like "6D 64 3E" or "6D643E"
 */
inline QByteArray hexStringToBytes(const QString& hex)
{
    QString clean = hex.simplified().remove(' ');
    return QByteArray::fromHex(clean.toLatin1());
}

/**
 * @brief Convert byte array to hex string.
 * @param bytes Raw bytes
 * @param separator Separator between bytes (default: space)
 */
inline QString bytesToHexString(const QByteArray& bytes, const QString& separator = " ")
{
    QStringList parts;
    parts.reserve(bytes.size());
    for (unsigned char byte : bytes) {
        parts.append(QString("%1").arg(byte, 2, 16, QChar('0')).toUpper());
    }
    return parts.join(separator);
}

} // namespace HexUtils

