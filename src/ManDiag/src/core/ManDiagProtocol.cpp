/**
 * @file ManDiagProtocol.cpp
 * @brief Shared parsing and matching utilities for ManDiag protocols.
 */

#include "core/ManDiagProtocol.h"
#include "HexUtils.h"
#include <QRegularExpression>

namespace ManDiag::Protocol {

namespace {

constexpr int kResponseFieldsAfterPrefix = 5;  // group, test, operation, status, len

bool isHexPair(const QString& token)
{
    static const QRegularExpression rx("^[0-9A-F]{2}$");
    return rx.match(token).hasMatch();
}

bool isHexChars(const QString& token, bool allowWildcard)
{
    static const QRegularExpression hexOnly("^[0-9A-F]+$");
    static const QRegularExpression hexOrWildcard("^[0-9A-FX]+$");
    return allowWildcard
        ? hexOrWildcard.match(token).hasMatch()
        : hexOnly.match(token).hasMatch();
}

int findLastPrefixIndex(const QStringList& tokens, const QStringList& prefix)
{
    if (prefix.isEmpty() || tokens.size() < prefix.size()) {
        return -1;
    }

    int lastIndex = -1;
    for (int i = 0; i <= tokens.size() - prefix.size(); ++i) {
        bool matched = true;
        for (int j = 0; j < prefix.size(); ++j) {
            if (tokens[i + j] != prefix[j]) {
                matched = false;
                break;
            }
        }
        if (matched) {
            lastIndex = i;
        }
    }

    return lastIndex;
}

Frame invalidFrame(const QString& raw, const QString& error)
{
    Frame frame;
    frame.valid = false;
    frame.raw = raw;
    frame.error = error;
    return frame;
}

Frame parseFrameTokens(const QStringList& tokens,
                       const QStringList& prefixBytes,
                       bool allowWildcards)
{
    Frame frame;
    frame.bytes = tokens;
    frame.normalized = normalizeTokens(tokens);

    const int prefixLen = prefixBytes.size();
    const int headerLen = prefixLen + kResponseFieldsAfterPrefix;

    if (tokens.size() < headerLen) {
        frame.error = QString("Frame too short. Expected at least %1 bytes, got %2")
                          .arg(headerLen)
                          .arg(tokens.size());
        return frame;
    }

    for (int i = 0; i < prefixLen; ++i) {
        if (tokens[i] != prefixBytes[i]) {
            frame.error = QString("Invalid prefix at byte %1. Expected %2, got %3")
                              .arg(i)
                              .arg(prefixBytes[i], tokens[i]);
            return frame;
        }
    }

    frame.groupId = tokens[prefixLen + 0];
    frame.testId = tokens[prefixLen + 1];
    frame.operation = tokens[prefixLen + 2];
    frame.statusByte = tokens[prefixLen + 3];
    frame.dataLengthByte = tokens[prefixLen + 4];

    bool ok = false;
    int dataLen = 0;
    if (allowWildcards && frame.dataLengthByte == "XX") {
        dataLen = -1;
        ok = true;
    } else {
        dataLen = frame.dataLengthByte.toInt(&ok, 16);
    }

    if (!ok) {
        frame.error = "Invalid data length byte: " + frame.dataLengthByte;
        return frame;
    }

    if (dataLen < 0) {
        frame.dataBytes = tokens.mid(headerLen);
        frame.valid = true;
        return frame;
    }

    const int expectedTotal = headerLen + dataLen;
    if (tokens.size() != expectedTotal) {
        frame.error = QString("Frame length mismatch. Expected %1 bytes from data length, got %2")
                          .arg(expectedTotal)
                          .arg(tokens.size());
        return frame;
    }

    frame.dataBytes = tokens.mid(headerLen, dataLen);
    frame.valid = true;
    return frame;
}

} // namespace

int Frame::dataLength() const
{
    bool ok = false;
    const int len = dataLengthByte.toInt(&ok, 16);
    return ok ? len : 0;
}

bool Frame::isPending() const
{
    return statusByte.compare(STATUS_PENDING, Qt::CaseInsensitive) == 0;
}

QVariantMap Frame::toVariantMap() const
{
    QVariantMap map;
    map["valid"] = valid;
    map["raw"] = raw;
    map["normalized"] = normalized;
    map["error"] = error;
    map["group_id"] = groupId;
    map["test_id"] = testId;
    map["operation"] = operation;
    map["status_byte"] = statusByte;
    map["data_length_byte"] = dataLengthByte;
    map["data_length"] = dataLength();
    map["data_bytes"] = dataBytes.join(" ");
    return map;
}

QStringList tokenizeHex(const QString& input, bool allowWildcard, QString* error)
{
    if (error) {
        error->clear();
    }

    const QStringList chunks = input.toUpper().split(
        QRegularExpression("[\\s,;:\\-]+"), Qt::SkipEmptyParts);

    QStringList tokens;
    for (QString chunk : chunks) {
        chunk = chunk.trimmed();
        if (chunk.startsWith("0X")) {
            chunk = chunk.mid(2);
        }

        if (chunk.isEmpty()) {
            continue;
        }

        if (!isHexChars(chunk, allowWildcard)) {
            if (error) {
                *error = "Invalid token: " + chunk;
            }
            return {};
        }

        if (chunk.size() % 2 != 0) {
            if (error) {
                *error = "Odd-length hex token: " + chunk;
            }
            return {};
        }

        for (int i = 0; i < chunk.size(); i += 2) {
            const QString pair = chunk.mid(i, 2);
            if (allowWildcard && pair == "XX") {
                tokens.append(pair);
                continue;
            }
            if (!isHexPair(pair)) {
                if (error) {
                    *error = "Invalid hex byte: " + pair;
                }
                return {};
            }
            tokens.append(pair);
        }
    }

    if (tokens.isEmpty() && !input.trimmed().isEmpty()) {
        if (error) {
            *error = "No valid hex bytes found";
        }
    }

    return tokens;
}

QString normalizeTokens(const QStringList& tokens, const QString& separator)
{
    QStringList normalized;
    normalized.reserve(tokens.size());
    for (const QString& token : tokens) {
        normalized.append(token.toUpper());
    }
    return normalized.join(separator);
}

QByteArray tokensToBytes(const QStringList& tokens)
{
    return QByteArray::fromHex(normalizeTokens(tokens, "").toLatin1());
}

bool tokenMatches(const QString& actual, const QString& expectedPattern)
{
    const QString expected = expectedPattern.trimmed().toUpper();
    if (expected == "XX") {
        return true;
    }
    return actual.trimmed().toUpper() == expected;
}

bool bytesMatchWithWildcards(const QStringList& actual,
                             const QStringList& expectedPattern,
                             QString* mismatchDetail,
                             bool requireSameLength)
{
    if (mismatchDetail) {
        mismatchDetail->clear();
    }

    if (requireSameLength && actual.size() != expectedPattern.size()) {
        if (mismatchDetail) {
            *mismatchDetail = QString("Length mismatch. Expected %1 bytes, got %2")
                                  .arg(expectedPattern.size())
                                  .arg(actual.size());
        }
        return false;
    }

    if (expectedPattern.size() > actual.size()) {
        if (mismatchDetail) {
            *mismatchDetail = QString("Insufficient bytes. Expected at least %1, got %2")
                                  .arg(expectedPattern.size())
                                  .arg(actual.size());
        }
        return false;
    }

    for (int i = 0; i < expectedPattern.size(); ++i) {
        if (!tokenMatches(actual[i], expectedPattern[i])) {
            if (mismatchDetail) {
                *mismatchDetail = QString("Byte %1 mismatch. Expected %2, got %3")
                                      .arg(i)
                                      .arg(expectedPattern[i], actual[i]);
            }
            return false;
        }
    }

    return true;
}

QStringList defaultPrefixBytes()
{
    return tokenizeHex(ITS_FRAME_PREFIX);
}

Frame parseFrame(const QString& frameHex,
                 const QStringList& prefixBytes,
                 bool allowWildcards)
{
    QString error;
    const QStringList tokens = tokenizeHex(frameHex, allowWildcards, &error);
    if (tokens.isEmpty()) {
        return invalidFrame(frameHex, error.isEmpty() ? "Empty frame" : error);
    }

    Frame frame = parseFrameTokens(tokens, prefixBytes, allowWildcards);
    frame.raw = frameHex;
    if (!frame.valid && frame.error.isEmpty()) {
        frame.error = "Unable to parse frame";
    }
    return frame;
}

Frame extractFrame(const QByteArray& rawBytes, const QStringList& prefixBytes)
{
    const QString rawHex = HexUtils::bytesToHexString(rawBytes);

    QString error;
    const QStringList tokens = tokenizeHex(rawHex, false, &error);
    if (tokens.isEmpty()) {
        return invalidFrame(rawHex, error.isEmpty() ? "No bytes available" : error);
    }

    const int prefixStart = findLastPrefixIndex(tokens, prefixBytes);
    if (prefixStart < 0) {
        return invalidFrame(rawHex, "Frame prefix not found");
    }

    const QStringList candidate = tokens.mid(prefixStart);
    const int prefixLen = prefixBytes.size();
    const int headerLen = prefixLen + kResponseFieldsAfterPrefix;
    if (candidate.size() < headerLen) {
        return invalidFrame(rawHex, "Partial frame received");
    }

    bool ok = false;
    const int dataLen = candidate[prefixLen + 4].toInt(&ok, 16);
    if (!ok) {
        return invalidFrame(rawHex, "Invalid data length byte in received frame");
    }

    const int requiredBytes = headerLen + dataLen;
    if (candidate.size() < requiredBytes) {
        return invalidFrame(rawHex, QString("Incomplete frame. Need %1 bytes, got %2")
                                        .arg(requiredBytes)
                                        .arg(candidate.size()));
    }

    const QStringList frameTokens = candidate.mid(0, requiredBytes);
    Frame frame = parseFrameTokens(frameTokens, prefixBytes, false);
    frame.raw = rawHex;
    if (!frame.valid && frame.error.isEmpty()) {
        frame.error = "Unable to parse extracted frame";
    }
    return frame;
}

} // namespace ManDiag::Protocol

