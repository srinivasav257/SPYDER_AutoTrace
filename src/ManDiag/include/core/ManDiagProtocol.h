#pragma once
/**
 * @file ManDiagProtocol.h
 * @brief Shared parsing and matching utilities for ManDiag protocols.
 */

#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVariantMap>

// Windows headers may define STATUS_SUCCESS/STATUS_PENDING macros.
#ifdef STATUS_SUCCESS
#undef STATUS_SUCCESS
#endif
#ifdef STATUS_PENDING
#undef STATUS_PENDING
#endif

namespace ManDiag::Protocol {

/// ITS frame prefix bytes represented as compact hex.
constexpr const char* ITS_FRAME_PREFIX = "6D643E";

/// Successful status byte.
constexpr const char* STATUS_SUCCESS = "01";

/// Pending status byte.
constexpr const char* STATUS_PENDING = "AA";

/// Default command timeout for ManDiag operations.
constexpr int DEFAULT_TIMEOUT_MS = 5000;

/// Default max pending-retry budget.
constexpr int DEFAULT_PENDING_TIMEOUT_MS = 15000;

/**
 * @brief Parsed ManDiag response frame.
 *
 * Expected binary layout:
 * [prefix bytes][group][test][operation][status][dataLength][data...]
 */
struct Frame {
    bool valid = false;
    QString raw;
    QString normalized;
    QString error;

    QString groupId;
    QString testId;
    QString operation;
    QString statusByte;
    QString dataLengthByte;
    QStringList dataBytes;
    QStringList bytes;

    int dataLength() const;
    bool isPending() const;
    QVariantMap toVariantMap() const;
};

/**
 * @brief Split mixed hex formats into uppercase byte tokens.
 *
 * Supported input styles:
 * - "6D643E 00 01 01 01 00"
 * - "6D 64 3E 00 01 01 01 00"
 * - "0x6D 0x64 0x3E ..."
 */
QStringList tokenizeHex(const QString& input, bool allowWildcard = false, QString* error = nullptr);

/**
 * @brief Join byte tokens into a normalized hex string.
 */
QString normalizeTokens(const QStringList& tokens, const QString& separator = " ");

/**
 * @brief Convert byte tokens to raw bytes.
 */
QByteArray tokensToBytes(const QStringList& tokens);

/**
 * @brief Byte token comparison with wildcard support ("XX").
 */
bool tokenMatches(const QString& actual, const QString& expectedPattern);

/**
 * @brief Compare token arrays with wildcard support.
 */
bool bytesMatchWithWildcards(const QStringList& actual,
                             const QStringList& expectedPattern,
                             QString* mismatchDetail = nullptr,
                             bool requireSameLength = true);

/**
 * @brief Prefix bytes for ITS protocol.
 */
QStringList defaultPrefixBytes();

/**
 * @brief Parse a frame provided as hex text.
 */
Frame parseFrame(const QString& frameHex,
                 const QStringList& prefixBytes = defaultPrefixBytes(),
                 bool allowWildcards = false);

/**
 * @brief Extract the last complete frame from a raw byte buffer.
 */
Frame extractFrame(const QByteArray& rawBytes,
                   const QStringList& prefixBytes = defaultPrefixBytes());

} // namespace ManDiag::Protocol
