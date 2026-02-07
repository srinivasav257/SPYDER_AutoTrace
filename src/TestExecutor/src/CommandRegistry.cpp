/**
 * @file CommandRegistry.cpp
 * @brief Implementation of Command Registry with built-in commands.
 * 
 * This file registers all predefined test commands that users can use
 * without writing code. Commands are grouped by category:
 * - Serial: ManDiag commands, UART communication
 * - CAN: CAN bus messaging
 * - Power: Power supply control
 * - Flow: Execution flow control (wait, loop, condition)
 * - Validation: Response validation and assertions
 * - System: File operations, logging
 */

#include "CommandRegistry.h"
#include <SerialManager.h>
#include <CANManager.h>
#include <CANInterface.h>
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QRegularExpression>
#include <QElapsedTimer>

using namespace SerialManager;

namespace TestExecutor {

//=============================================================================
// Utility Functions (hexStringToBytes/bytesToHexString are now inline in header via HexUtils)
//=============================================================================

//=============================================================================
// CommandRegistry Singleton
//=============================================================================

CommandRegistry& CommandRegistry::instance()
{
    static CommandRegistry instance;
    return instance;
}

CommandRegistry::CommandRegistry() = default;

bool CommandRegistry::registerCommand(const CommandDef& cmd)
{
    if (m_commands.contains(cmd.id)) {
        qWarning() << "Command already registered:" << cmd.id;
        return false;
    }
    m_commands[cmd.id] = cmd;
    return true;
}

bool CommandRegistry::registerCommand(CommandDef&& cmd)
{
    if (m_commands.contains(cmd.id)) {
        qWarning() << "Command already registered:" << cmd.id;
        return false;
    }
    m_commands[cmd.id] = std::move(cmd);
    return true;
}

void CommandRegistry::registerBuiltinCommands()
{
    registerSerialCommands();
    registerCANCommands();
    registerPowerCommands();
    registerFlowCommands();
    registerValidationCommands();
    registerSystemCommands();
    
    qDebug() << "Registered" << m_commands.size() << "builtin commands";
}

const CommandDef* CommandRegistry::command(const QString& id) const
{
    auto it = m_commands.constFind(id);
    if (it == m_commands.constEnd()) {
        return nullptr;
    }
    return &it.value();
}

QList<CommandDef> CommandRegistry::commandsByCategory(CommandCategory category) const
{
    QList<CommandDef> result;
    for (const auto& cmd : m_commands) {
        if (cmd.category == category) {
            result.append(cmd);
        }
    }
    return result;
}

QList<CommandCategory> CommandRegistry::categories() const
{
    QSet<CommandCategory> cats;
    for (const auto& cmd : m_commands) {
        cats.insert(cmd.category);
    }
    return cats.values();
}

QStringList CommandRegistry::commandNamesForCategory(CommandCategory category) const
{
    QStringList names;
    for (const auto& cmd : m_commands) {
        if (cmd.category == category) {
            names.append(cmd.name);
        }
    }
    return names;
}

CommandResult CommandRegistry::execute(const QString& commandId,
                                        const QVariantMap& params,
                                        const QVariantMap& config,
                                        const std::atomic<bool>* cancel)
{
    const CommandDef* cmd = command(commandId);
    if (!cmd) {
        return CommandResult::Failure("Unknown command: " + commandId);
    }
    
    // Validate parameters
    QString validationError = validateParameters(commandId, params);
    if (!validationError.isEmpty()) {
        return CommandResult::Failure("Parameter validation failed: " + validationError);
    }
    
    // Check cancellation before executing
    if (cancel && cancel->load()) {
        return CommandResult::Failure("Cancelled before execution");
    }
    
    // Execute the command
    try {
        return cmd->handler(params, config, cancel);
    } catch (const std::exception& e) {
        return CommandResult::Failure(QString("Exception: %1").arg(e.what()));
    }
}

QString CommandRegistry::validateParameters(const QString& commandId, const QVariantMap& params) const
{
    const CommandDef* cmd = command(commandId);
    if (!cmd) {
        return "Unknown command";
    }
    
    for (const auto& paramDef : cmd->parameters) {
        if (paramDef.required && !params.contains(paramDef.name)) {
            return QString("Missing required parameter: %1").arg(paramDef.displayName);
        }
    }
    
    return QString(); // Valid
}

//=============================================================================
// Serial Commands Registration
//=============================================================================

void CommandRegistry::registerSerialCommands()
{
    // Enter Manufacturing Diagnostic Session
    registerCommand({
        .id = "serial_enter_md_session",
        .name = "Enter MD Session",
        .description = "Enter manufacturing diagnostic session via UART",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "session_type",
                .displayName = "Session Type",
                .description = "Type of diagnostic session to enter",
                .type = ParameterType::Enum,
                .defaultValue = "manufacturing",
                .required = true,
                .enumValues = {"manufacturing", "extended", "default"}
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 100,
                .maxValue = 60000,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            // TODO: Implement actual serial communication
            QString sessionType = params.value("session_type", "manufacturing").toString();
            qDebug() << "Entering" << sessionType << "diagnostic session";
            return CommandResult::Success("Entered " + sessionType + " session");
        }
    });
    
    // Exit MD Session
    registerCommand({
        .id = "serial_exit_md_session",
        .name = "Exit MD Session",
        .description = "Exit manufacturing diagnostic session",
        .category = CommandCategory::Serial,
        .parameters = {},
        .handler = [](const QVariantMap& /*params*/, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            qDebug() << "Exiting diagnostic session";
            return CommandResult::Success("Exited diagnostic session");
        }
    });
    
    // Send ManDiag Command
    registerCommand({
        .id = "serial_send_mandiag",
        .name = "Send ManDiag Command",
        .description = "Send a manufacturing diagnostic command via UART",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "hex_command",
                .displayName = "Command (Hex)",
                .description = "Hex bytes to send (e.g., '6D 64 3E 64 02 00 00 01 00')",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E",
                .required = true
            },
            {
                .name = "expected_response",
                .displayName = "Expected Response",
                .description = "Pattern to match in response",
                .type = ParameterType::String,
                .defaultValue = "OK",
                .required = false
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 100,
                .maxValue = 60000,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString hexCmd = params.value("hex_command").toString();
            QByteArray cmdBytes = hexStringToBytes(hexCmd);
            qDebug() << "Sending ManDiag:" << bytesToHexString(cmdBytes);
            // TODO: Implement actual serial communication
            return CommandResult::Success("Command sent: " + hexCmd);
        }
    });
    
    // Read DID
    registerCommand({
        .id = "serial_read_did",
        .name = "Read DID",
        .description = "Read a Data Identifier from the ECU",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "did_id",
                .displayName = "DID",
                .description = "Data Identifier to read (hex, e.g., 'F190')",
                .type = ParameterType::HexString,
                .defaultValue = "F190",
                .required = true
            },
            {
                .name = "expected_length",
                .displayName = "Expected Length",
                .description = "Expected response data length in bytes",
                .type = ParameterType::Integer,
                .defaultValue = 0,
                .required = false,
                .minValue = 0,
                .maxValue = 4096
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 100,
                .maxValue = 60000,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString did = params.value("did_id").toString();
            qDebug() << "Reading DID:" << did;
            // TODO: Implement actual DID read
            QVariantMap response;
            response["did"] = did;
            response["data"] = "Sample DID Data";
            return CommandResult::Success("Read DID " + did, response);
        }
    });
    
    // Write DID
    registerCommand({
        .id = "serial_write_did",
        .name = "Write DID",
        .description = "Write a Data Identifier to the ECU",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "did_id",
                .displayName = "DID",
                .description = "Data Identifier to write (hex)",
                .type = ParameterType::HexString,
                .defaultValue = "F190",
                .required = true
            },
            {
                .name = "data",
                .displayName = "Data",
                .description = "Data to write (hex bytes)",
                .type = ParameterType::HexString,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString did = params.value("did_id").toString();
            QString data = params.value("data").toString();
            qDebug() << "Writing DID:" << did << "Data:" << data;
            // TODO: Implement actual DID write
            return CommandResult::Success("Wrote DID " + did);
        }
    });
    
    // Send Raw Serial
    registerCommand({
        .id = "serial_send_raw",
        .name = "Send Raw Serial",
        .description = "Send raw data via serial port",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "data",
                .displayName = "Data",
                .description = "Data to send (hex bytes or ASCII text)",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "data_format",
                .displayName = "Format",
                .description = "Data format",
                .type = ParameterType::Enum,
                .defaultValue = "hex",
                .required = true,
                .enumValues = {"hex", "ascii"}
            },
            {
                .name = "add_newline",
                .displayName = "Add Newline",
                .description = "Append CR/LF to data",
                .type = ParameterType::Boolean,
                .defaultValue = true,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString data = params.value("data").toString();
            QString format = params.value("data_format", "hex").toString();
            qDebug() << "Sending raw serial:" << data << "format:" << format;
            return CommandResult::Success("Sent data");
        }
    });
    
    // Read Serial Response
    registerCommand({
        .id = "serial_read_response",
        .name = "Read Serial Response",
        .description = "Read data from serial port",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for data",
                .type = ParameterType::Duration,
                .defaultValue = 1000,
                .required = false,
                .unit = "ms"
            },
            {
                .name = "expected_pattern",
                .displayName = "Expected Pattern",
                .description = "Regex pattern to match in response",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            int timeout = params.value("timeout_ms", 1000).toInt();
            qDebug() << "Reading serial response, timeout:" << timeout;
            QVariantMap response;
            response["data"] = "Sample response data";
            return CommandResult::Success("Read response", response);
        }
    });
    
    // Serial Send - Send data on serial terminal
    registerCommand({
        .id = "serial_send",
        .name = "Serial Send",
        .description = "Send data string to serial port",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "port",
                .displayName = "Port",
                .description = "Serial port to send data (e.g., COM1, /dev/ttyUSB0)",
                .type = ParameterType::ComPort,
                .defaultValue = "COM1",
                .required = true
            },
            {
                .name = "data_string",
                .displayName = "Data String",
                .description = "Data to send to serial port",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString port = params.value("port").toString();
            QString dataString = params.value("data_string").toString();
            
            qDebug() << "Serial Send - Port:" << port << "Data:" << dataString;
            
            // Get SerialManager singleton - uses existing connection if port is open
            auto& serialMgr = SerialPortManager::instance();
            
            // Send data - SerialManager will auto-open port using stored config if not open
            SerialResult result = serialMgr.send(port, dataString);
            
            QVariantMap response;
            response["port"] = port;
            response["data_sent"] = dataString;
            response["bytes_sent"] = result.bytesWritten;
            
            if (result.success) {
                return CommandResult::Success("Data sent to " + port, response);
            } else {
                return CommandResult::Failure("Failed to send: " + result.errorMessage);
            }
        }
    });
    
    // Serial Send Match Response - Send command and check for specific response
    registerCommand({
        .id = "serial_send_match_response",
        .name = "Serial Send Match Response",
        .description = "Send command via serial and check for specific string in response within timeout",
        .category = CommandCategory::Serial,
        .parameters = {
            {
                .name = "port",
                .displayName = "Port",
                .description = "Serial port to use (e.g., COM1, /dev/ttyUSB0)",
                .type = ParameterType::ComPort,
                .defaultValue = "COM1",
                .required = true
            },
            {
                .name = "data_string",
                .displayName = "Data String",
                .description = "Command/data to send to serial port",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "response_string",
                .displayName = "Response String",
                .description = "Expected string to find in serial response",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for expected response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = true,
                .minValue = 100,
                .maxValue = 300000,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString port = params.value("port").toString();
            QString dataString = params.value("data_string").toString();
            QString responseString = params.value("response_string").toString();
            int timeoutMs = params.value("timeout_ms", 5000).toInt();
            
            qDebug() << "Serial Send Match Response - Port:" << port 
                     << "Data:" << dataString 
                     << "Expected:" << responseString
                     << "Timeout:" << timeoutMs << "ms";
            
            // Get SerialManager singleton - uses existing connection if port is open
            auto& serialMgr = SerialPortManager::instance();
            
            // Send data and wait for matching response
            SerialResult result = serialMgr.sendAndMatchResponse(
                port, dataString, responseString, timeoutMs);
            
            QVariantMap response;
            response["port"] = port;
            response["data_sent"] = dataString;
            response["expected_response"] = responseString;
            response["timeout_ms"] = timeoutMs;
            response["received_data"] = QString::fromUtf8(result.data);
            response["match_found"] = result.matchFound;
            
            if (result.success && result.matchFound) {
                return CommandResult::Success("Response matched: " + responseString, response);
            } else {
                return CommandResult::Failure(result.errorMessage.isEmpty() 
                    ? "Response not matched within timeout" 
                    : result.errorMessage);
            }
        }
    });
}

//=============================================================================
// CAN Commands Registration
//=============================================================================

/**
 * @brief Helper: Build a CANMessage from common parameter map values.
 */
static CANManager::CANMessage buildCANMessage(const QVariantMap& params, bool isFD)
{
    CANManager::CANMessage msg{};

    // Parse CAN ID (supports "0x1A3", "1A3", or decimal)
    QString idStr = params.value("can_id").toString().trimmed();
    bool ok = false;
    if (idStr.startsWith("0x", Qt::CaseInsensitive))
        msg.id = idStr.mid(2).toUInt(&ok, 16);
    else
        msg.id = idStr.toUInt(&ok, 16);
    if (!ok) msg.id = idStr.toUInt(); // fallback decimal

    msg.isExtended = params.value("extended_id", false).toBool();
    msg.isFD       = isFD;
    msg.isBRS      = isFD; // enable bit-rate switch when FD

    // Parse payload hex → bytes
    QByteArray payload = hexStringToBytes(params.value("data").toString());
    int maxLen = isFD ? 64 : 8;
    int len    = qMin(payload.size(), maxLen);
    std::memcpy(msg.data, payload.constData(), len);
    msg.dlc = CANManager::lengthToDlc(len);

    return msg;
}

/**
 * @brief Helper: Receive a CAN response matching a specific ID within timeout.
 * Flushes the RX queue first, then polls until a matching frame arrives.
 */
static CANManager::CANResult receiveMatchingId(const QString& slotName,
                                                uint32_t targetId,
                                                CANManager::CANMessage& rxMsg,
                                                int timeoutMs)
{
    auto& can = CANManager::CANBusManager::instance();
    can.flushReceiveQueue(slotName);

    QElapsedTimer timer;
    timer.start();

    while (!timer.hasExpired(timeoutMs)) {
        int remaining = timeoutMs - static_cast<int>(timer.elapsed());
        if (remaining <= 0) break;

        auto result = can.receive(slotName, rxMsg, qMin(remaining, 100));
        if (result.success && rxMsg.id == targetId) {
            return CANManager::CANResult::Success();
        }
    }
    return CANManager::CANResult::Failure(
        QString("No response with CAN ID 0x%1 within %2 ms")
            .arg(targetId, 0, 16).arg(timeoutMs));
}

void CommandRegistry::registerCANCommands()
{
    // =========================================================================
    // 1. CANHS_Tx – CAN High-Speed Transmit (8 bytes, fire-and-forget)
    // =========================================================================
    registerCommand({
        .id = "canhs_tx",
        .name = "CANHS_Tx",
        .description = "Transmit a Classic CAN (High-Speed) message – fire and forget (max 8 bytes)",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "slot",
                .displayName = "CAN Slot",
                .description = "Logical CAN slot name configured in HW Config (e.g. 'CAN 1')",
                .type = ParameterType::String,
                .defaultValue = "CAN 1",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "CAN ID",
                .description = "Arbitration ID in hex (e.g. '0x7E0')",
                .type = ParameterType::CanId,
                .defaultValue = "0x100",
                .required = true
            },
            {
                .name = "data",
                .displayName = "Data (Hex)",
                .description = "Payload bytes in hex (max 8 bytes, e.g. '02 10 01 00 00 00 00 00')",
                .type = ParameterType::HexString,
                .defaultValue = "00 00 00 00 00 00 00 00",
                .required = true
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended CAN ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString slot = params.value("slot", "CAN 1").toString();
            auto& can = CANManager::CANBusManager::instance();

            if (!can.isSlotOpen(slot))
                return CommandResult::Failure("CAN slot '" + slot + "' is not open");

            CANManager::CANMessage msg = buildCANMessage(params, /*isFD=*/false);
            auto result = can.transmit(slot, msg);

            QVariantMap resp;
            resp["can_id"]  = QString("0x%1").arg(msg.id, 0, 16).toUpper();
            resp["data"]    = bytesToHexString(QByteArray(reinterpret_cast<const char*>(msg.data), msg.dataLength()));
            resp["dlc"]     = msg.dlc;

            if (result.success)
                return CommandResult::Success("CAN HS message transmitted", resp);
            else
                return CommandResult::Failure("Transmit failed: " + result.errorMessage);
        }
    });

    // =========================================================================
    // 2. CANFD_Tx – CAN FD Transmit (up to 64 bytes, fire-and-forget)
    // =========================================================================
    registerCommand({
        .id = "canfd_tx",
        .name = "CANFD_Tx",
        .description = "Transmit a CAN FD message – fire and forget (up to 64 bytes)",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "slot",
                .displayName = "CAN Slot",
                .description = "Logical CAN slot name configured in HW Config (e.g. 'CAN 1')",
                .type = ParameterType::String,
                .defaultValue = "CAN 1",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "CAN ID",
                .description = "Arbitration ID in hex (e.g. '0x7E0')",
                .type = ParameterType::CanId,
                .defaultValue = "0x100",
                .required = true
            },
            {
                .name = "data",
                .displayName = "Data (Hex)",
                .description = "Payload bytes in hex (up to 64 bytes for CAN FD)",
                .type = ParameterType::HexString,
                .defaultValue = "00 00 00 00 00 00 00 00",
                .required = true
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended CAN ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString slot = params.value("slot", "CAN 1").toString();
            auto& can = CANManager::CANBusManager::instance();

            if (!can.isSlotOpen(slot))
                return CommandResult::Failure("CAN slot '" + slot + "' is not open");

            CANManager::CANMessage msg = buildCANMessage(params, /*isFD=*/true);
            auto result = can.transmit(slot, msg);

            QVariantMap resp;
            resp["can_id"]  = QString("0x%1").arg(msg.id, 0, 16).toUpper();
            resp["data"]    = bytesToHexString(QByteArray(reinterpret_cast<const char*>(msg.data), msg.dataLength()));
            resp["dlc"]     = msg.dlc;
            resp["is_fd"]   = true;

            if (result.success)
                return CommandResult::Success("CAN FD message transmitted", resp);
            else
                return CommandResult::Failure("Transmit failed: " + result.errorMessage);
        }
    });

    // =========================================================================
    // 3. CANHS_TxRx – CAN HS Send and Collect Response
    // =========================================================================
    registerCommand({
        .id = "canhs_txrx",
        .name = "CANHS_TxRx",
        .description = "Transmit a Classic CAN message and collect the response (request/response)",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "slot",
                .displayName = "CAN Slot",
                .description = "Logical CAN slot name (e.g. 'CAN 1')",
                .type = ParameterType::String,
                .defaultValue = "CAN 1",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "TX CAN ID",
                .description = "Transmit arbitration ID in hex (e.g. '0x7E0')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E0",
                .required = true
            },
            {
                .name = "data",
                .displayName = "TX Data (Hex)",
                .description = "Payload bytes to send (max 8 bytes)",
                .type = ParameterType::HexString,
                .defaultValue = "02 10 01 00 00 00 00 00",
                .required = true
            },
            {
                .name = "rx_can_id",
                .displayName = "RX CAN ID",
                .description = "Expected response CAN ID in hex (e.g. '0x7E8')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E8",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Max time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 50,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended CAN ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString slot = params.value("slot", "CAN 1").toString();
            int timeoutMs = params.value("timeout_ms", 5000).toInt();
            auto& can = CANManager::CANBusManager::instance();

            if (!can.isSlotOpen(slot))
                return CommandResult::Failure("CAN slot '" + slot + "' is not open");

            // --- Transmit ---
            CANManager::CANMessage txMsg = buildCANMessage(params, /*isFD=*/false);
            auto txResult = can.transmit(slot, txMsg);
            if (!txResult.success)
                return CommandResult::Failure("Transmit failed: " + txResult.errorMessage);

            // --- Parse RX CAN ID ---
            QString rxIdStr = params.value("rx_can_id").toString().trimmed();
            bool ok = false;
            uint32_t rxId = rxIdStr.startsWith("0x", Qt::CaseInsensitive)
                ? rxIdStr.mid(2).toUInt(&ok, 16)
                : rxIdStr.toUInt(&ok, 16);
            if (!ok) rxId = rxIdStr.toUInt();

            // --- Receive ---
            CANManager::CANMessage rxMsg{};
            auto rxResult = receiveMatchingId(slot, rxId, rxMsg, timeoutMs);

            QVariantMap resp;
            resp["tx_can_id"] = QString("0x%1").arg(txMsg.id, 0, 16).toUpper();
            resp["tx_data"]   = bytesToHexString(QByteArray(reinterpret_cast<const char*>(txMsg.data), txMsg.dataLength()));
            resp["rx_can_id"] = QString("0x%1").arg(rxMsg.id, 0, 16).toUpper();
            resp["rx_data"]   = bytesToHexString(QByteArray(reinterpret_cast<const char*>(rxMsg.data), rxMsg.dataLength()));
            resp["rx_dlc"]    = rxMsg.dlc;

            if (rxResult.success)
                return CommandResult::Success("Response received", resp);
            else
                return CommandResult::Failure(rxResult.errorMessage);
        }
    });

    // =========================================================================
    // 4. CANFD_TxRx – CAN FD Send and Collect Response
    // =========================================================================
    registerCommand({
        .id = "canfd_txrx",
        .name = "CANFD_TxRx",
        .description = "Transmit a CAN FD message and collect the response (request/response)",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "slot",
                .displayName = "CAN Slot",
                .description = "Logical CAN slot name (e.g. 'CAN 1')",
                .type = ParameterType::String,
                .defaultValue = "CAN 1",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "TX CAN ID",
                .description = "Transmit arbitration ID in hex (e.g. '0x7E0')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E0",
                .required = true
            },
            {
                .name = "data",
                .displayName = "TX Data (Hex)",
                .description = "Payload bytes to send (up to 64 bytes for FD)",
                .type = ParameterType::HexString,
                .defaultValue = "02 10 01 00 00 00 00 00",
                .required = true
            },
            {
                .name = "rx_can_id",
                .displayName = "RX CAN ID",
                .description = "Expected response CAN ID in hex (e.g. '0x7E8')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E8",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Max time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 50,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended CAN ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString slot = params.value("slot", "CAN 1").toString();
            int timeoutMs = params.value("timeout_ms", 5000).toInt();
            auto& can = CANManager::CANBusManager::instance();

            if (!can.isSlotOpen(slot))
                return CommandResult::Failure("CAN slot '" + slot + "' is not open");

            // --- Transmit (FD) ---
            CANManager::CANMessage txMsg = buildCANMessage(params, /*isFD=*/true);
            auto txResult = can.transmit(slot, txMsg);
            if (!txResult.success)
                return CommandResult::Failure("Transmit failed: " + txResult.errorMessage);

            // --- Parse RX CAN ID ---
            QString rxIdStr = params.value("rx_can_id").toString().trimmed();
            bool ok = false;
            uint32_t rxId = rxIdStr.startsWith("0x", Qt::CaseInsensitive)
                ? rxIdStr.mid(2).toUInt(&ok, 16)
                : rxIdStr.toUInt(&ok, 16);
            if (!ok) rxId = rxIdStr.toUInt();

            // --- Receive ---
            CANManager::CANMessage rxMsg{};
            auto rxResult = receiveMatchingId(slot, rxId, rxMsg, timeoutMs);

            QVariantMap resp;
            resp["tx_can_id"] = QString("0x%1").arg(txMsg.id, 0, 16).toUpper();
            resp["tx_data"]   = bytesToHexString(QByteArray(reinterpret_cast<const char*>(txMsg.data), txMsg.dataLength()));
            resp["rx_can_id"] = QString("0x%1").arg(rxMsg.id, 0, 16).toUpper();
            resp["rx_data"]   = bytesToHexString(QByteArray(reinterpret_cast<const char*>(rxMsg.data), rxMsg.dataLength()));
            resp["rx_dlc"]    = rxMsg.dlc;
            resp["is_fd"]     = true;

            if (rxResult.success)
                return CommandResult::Success("FD response received", resp);
            else
                return CommandResult::Failure(rxResult.errorMessage);
        }
    });

    // =========================================================================
    // 5. CANHS_TxRX – CAN HS Send, Collect Response & Match Expected
    // =========================================================================
    registerCommand({
        .id = "canhs_txrx_match",
        .name = "CANHS_TxRX",
        .description = "Transmit a Classic CAN message, collect the response, and compare with expected data",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "slot",
                .displayName = "CAN Slot",
                .description = "Logical CAN slot name (e.g. 'CAN 1')",
                .type = ParameterType::String,
                .defaultValue = "CAN 1",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "TX CAN ID",
                .description = "Transmit arbitration ID in hex (e.g. '0x7E0')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E0",
                .required = true
            },
            {
                .name = "data",
                .displayName = "TX Data (Hex)",
                .description = "Payload bytes to send (max 8 bytes)",
                .type = ParameterType::HexString,
                .defaultValue = "02 10 01 00 00 00 00 00",
                .required = true
            },
            {
                .name = "rx_can_id",
                .displayName = "RX CAN ID",
                .description = "Expected response CAN ID in hex (e.g. '0x7E8')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E8",
                .required = true
            },
            {
                .name = "expected_response",
                .displayName = "Expected Response (Hex)",
                .description = "Expected response payload in hex (e.g. '06 50 01 00 19 01 F4'). Use 'XX' for don't-care bytes.",
                .type = ParameterType::HexString,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Max time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 50,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended CAN ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString slot = params.value("slot", "CAN 1").toString();
            int timeoutMs = params.value("timeout_ms", 5000).toInt();
            auto& can = CANManager::CANBusManager::instance();

            if (!can.isSlotOpen(slot))
                return CommandResult::Failure("CAN slot '" + slot + "' is not open");

            // --- Transmit ---
            CANManager::CANMessage txMsg = buildCANMessage(params, /*isFD=*/false);
            auto txResult = can.transmit(slot, txMsg);
            if (!txResult.success)
                return CommandResult::Failure("Transmit failed: " + txResult.errorMessage);

            // --- Parse RX CAN ID ---
            QString rxIdStr = params.value("rx_can_id").toString().trimmed();
            bool ok = false;
            uint32_t rxId = rxIdStr.startsWith("0x", Qt::CaseInsensitive)
                ? rxIdStr.mid(2).toUInt(&ok, 16)
                : rxIdStr.toUInt(&ok, 16);
            if (!ok) rxId = rxIdStr.toUInt();

            // --- Receive ---
            CANManager::CANMessage rxMsg{};
            auto rxResult = receiveMatchingId(slot, rxId, rxMsg, timeoutMs);

            QByteArray rxPayload(reinterpret_cast<const char*>(rxMsg.data), rxMsg.dataLength());
            QString rxHex = bytesToHexString(rxPayload);

            QVariantMap resp;
            resp["tx_can_id"]      = QString("0x%1").arg(txMsg.id, 0, 16).toUpper();
            resp["tx_data"]        = bytesToHexString(QByteArray(reinterpret_cast<const char*>(txMsg.data), txMsg.dataLength()));
            resp["rx_can_id"]      = QString("0x%1").arg(rxMsg.id, 0, 16).toUpper();
            resp["rx_data"]        = rxHex;
            resp["rx_dlc"]         = rxMsg.dlc;

            if (!rxResult.success)
                return CommandResult::Failure(rxResult.errorMessage);

            // --- Match expected response ---
            QString expectedHex = params.value("expected_response").toString().trimmed();
            QByteArray expectedBytes = hexStringToBytes(expectedHex);

            // Support 'XX' wildcard bytes – compare only non-wildcard positions
            QStringList expectedTokens = expectedHex.toUpper().split(QRegularExpression("[\\s\\-:]+"), Qt::SkipEmptyParts);
            bool matched = (expectedTokens.size() <= rxPayload.size());
            QString mismatchDetail;
            for (int i = 0; matched && i < expectedTokens.size(); ++i) {
                if (expectedTokens[i] == "XX") continue;  // wildcard
                bool tokOk = false;
                uint8_t expByte = static_cast<uint8_t>(expectedTokens[i].toUInt(&tokOk, 16));
                if (!tokOk) { matched = false; mismatchDetail = "Invalid hex token: " + expectedTokens[i]; break; }
                if (static_cast<uint8_t>(rxPayload[i]) != expByte) {
                    matched = false;
                    mismatchDetail = QString("Byte %1: expected 0x%2, got 0x%3")
                        .arg(i)
                        .arg(expByte, 2, 16, QChar('0'))
                        .arg(static_cast<uint8_t>(rxPayload[i]), 2, 16, QChar('0'));
                }
            }

            resp["expected_data"]   = expectedHex;
            resp["match"]          = matched;

            if (matched)
                return CommandResult::Success("Response matches expected", resp);
            else
                return CommandResult::Failure("Response mismatch – " + mismatchDetail);
        }
    });

    // =========================================================================
    // 6. CANFD_TxRX – CAN FD Send, Collect Response & Match Expected
    // =========================================================================
    registerCommand({
        .id = "canfd_txrx_match",
        .name = "CANFD_TxRX",
        .description = "Transmit a CAN FD message, collect the response, and compare with expected data",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "slot",
                .displayName = "CAN Slot",
                .description = "Logical CAN slot name (e.g. 'CAN 1')",
                .type = ParameterType::String,
                .defaultValue = "CAN 1",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "TX CAN ID",
                .description = "Transmit arbitration ID in hex (e.g. '0x7E0')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E0",
                .required = true
            },
            {
                .name = "data",
                .displayName = "TX Data (Hex)",
                .description = "Payload bytes to send (up to 64 bytes for FD)",
                .type = ParameterType::HexString,
                .defaultValue = "02 10 01 00 00 00 00 00",
                .required = true
            },
            {
                .name = "rx_can_id",
                .displayName = "RX CAN ID",
                .description = "Expected response CAN ID in hex (e.g. '0x7E8')",
                .type = ParameterType::CanId,
                .defaultValue = "0x7E8",
                .required = true
            },
            {
                .name = "expected_response",
                .displayName = "Expected Response (Hex)",
                .description = "Expected response payload in hex. Use 'XX' for don't-care bytes.",
                .type = ParameterType::HexString,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Max time to wait for response",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .minValue = 50,
                .maxValue = 60000,
                .unit = "ms"
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended CAN ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString slot = params.value("slot", "CAN 1").toString();
            int timeoutMs = params.value("timeout_ms", 5000).toInt();
            auto& can = CANManager::CANBusManager::instance();

            if (!can.isSlotOpen(slot))
                return CommandResult::Failure("CAN slot '" + slot + "' is not open");

            // --- Transmit (FD) ---
            CANManager::CANMessage txMsg = buildCANMessage(params, /*isFD=*/true);
            auto txResult = can.transmit(slot, txMsg);
            if (!txResult.success)
                return CommandResult::Failure("Transmit failed: " + txResult.errorMessage);

            // --- Parse RX CAN ID ---
            QString rxIdStr = params.value("rx_can_id").toString().trimmed();
            bool ok = false;
            uint32_t rxId = rxIdStr.startsWith("0x", Qt::CaseInsensitive)
                ? rxIdStr.mid(2).toUInt(&ok, 16)
                : rxIdStr.toUInt(&ok, 16);
            if (!ok) rxId = rxIdStr.toUInt();

            // --- Receive ---
            CANManager::CANMessage rxMsg{};
            auto rxResult = receiveMatchingId(slot, rxId, rxMsg, timeoutMs);

            QByteArray rxPayload(reinterpret_cast<const char*>(rxMsg.data), rxMsg.dataLength());
            QString rxHex = bytesToHexString(rxPayload);

            QVariantMap resp;
            resp["tx_can_id"]      = QString("0x%1").arg(txMsg.id, 0, 16).toUpper();
            resp["tx_data"]        = bytesToHexString(QByteArray(reinterpret_cast<const char*>(txMsg.data), txMsg.dataLength()));
            resp["rx_can_id"]      = QString("0x%1").arg(rxMsg.id, 0, 16).toUpper();
            resp["rx_data"]        = rxHex;
            resp["rx_dlc"]         = rxMsg.dlc;
            resp["is_fd"]          = true;

            if (!rxResult.success)
                return CommandResult::Failure(rxResult.errorMessage);

            // --- Match expected response ---
            QString expectedHex = params.value("expected_response").toString().trimmed();
            QStringList expectedTokens = expectedHex.toUpper().split(QRegularExpression("[\\s\\-:]+"), Qt::SkipEmptyParts);
            bool matched = (expectedTokens.size() <= rxPayload.size());
            QString mismatchDetail;
            for (int i = 0; matched && i < expectedTokens.size(); ++i) {
                if (expectedTokens[i] == "XX") continue;
                bool tokOk = false;
                uint8_t expByte = static_cast<uint8_t>(expectedTokens[i].toUInt(&tokOk, 16));
                if (!tokOk) { matched = false; mismatchDetail = "Invalid hex token: " + expectedTokens[i]; break; }
                if (static_cast<uint8_t>(rxPayload[i]) != expByte) {
                    matched = false;
                    mismatchDetail = QString("Byte %1: expected 0x%2, got 0x%3")
                        .arg(i)
                        .arg(expByte, 2, 16, QChar('0'))
                        .arg(static_cast<uint8_t>(rxPayload[i]), 2, 16, QChar('0'));
                }
            }

            resp["expected_data"]   = expectedHex;
            resp["match"]          = matched;

            if (matched)
                return CommandResult::Success("FD response matches expected", resp);
            else
                return CommandResult::Failure("FD response mismatch – " + mismatchDetail);
        }
    });
}

//=============================================================================
// Power Commands Registration
//=============================================================================

void CommandRegistry::registerPowerCommands()
{
    // Turn ON Power Supply
    registerCommand({
        .id = "power_turn_on",
        .name = "Turn ON Power",
        .description = "Turn on the programmable power supply output",
        .category = CommandCategory::Power,
        .parameters = {
            {
                .name = "channel",
                .displayName = "Channel",
                .description = "Power supply channel",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = false,
                .minValue = 1,
                .maxValue = 4
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            int channel = params.value("channel", 1).toInt();
            qDebug() << "Turning ON power supply channel:" << channel;
            return CommandResult::Success("Power supply turned ON");
        }
    });
    
    // Turn OFF Power Supply
    registerCommand({
        .id = "power_turn_off",
        .name = "Turn OFF Power",
        .description = "Turn off the programmable power supply output",
        .category = CommandCategory::Power,
        .parameters = {
            {
                .name = "channel",
                .displayName = "Channel",
                .description = "Power supply channel",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = false,
                .minValue = 1,
                .maxValue = 4
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            int channel = params.value("channel", 1).toInt();
            qDebug() << "Turning OFF power supply channel:" << channel;
            return CommandResult::Success("Power supply turned OFF");
        }
    });
    
    // Set Voltage
    registerCommand({
        .id = "power_set_voltage",
        .name = "Set Voltage",
        .description = "Set the output voltage of the power supply",
        .category = CommandCategory::Power,
        .parameters = {
            {
                .name = "voltage",
                .displayName = "Voltage",
                .description = "Output voltage",
                .type = ParameterType::Voltage,
                .defaultValue = 12.0,
                .required = true,
                .minValue = 0.0,
                .maxValue = 60.0,
                .unit = "V"
            },
            {
                .name = "channel",
                .displayName = "Channel",
                .description = "Power supply channel",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = false,
                .minValue = 1,
                .maxValue = 4
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            double voltage = params.value("voltage", 12.0).toDouble();
            int channel = params.value("channel", 1).toInt();
            qDebug() << "Setting voltage to" << voltage << "V on channel" << channel;
            return CommandResult::Success(QString("Voltage set to %1V").arg(voltage));
        }
    });
    
    // Set Current Limit
    registerCommand({
        .id = "power_set_current",
        .name = "Set Current Limit",
        .description = "Set the current limit of the power supply",
        .category = CommandCategory::Power,
        .parameters = {
            {
                .name = "current",
                .displayName = "Current Limit",
                .description = "Maximum current",
                .type = ParameterType::Current,
                .defaultValue = 5.0,
                .required = true,
                .minValue = 0.0,
                .maxValue = 30.0,
                .unit = "A"
            },
            {
                .name = "channel",
                .displayName = "Channel",
                .description = "Power supply channel",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = false,
                .minValue = 1,
                .maxValue = 4
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            double current = params.value("current", 5.0).toDouble();
            qDebug() << "Setting current limit to" << current << "A";
            return CommandResult::Success(QString("Current limit set to %1A").arg(current));
        }
    });
    
    // Read Measurements
    registerCommand({
        .id = "power_read_measurements",
        .name = "Read Measurements",
        .description = "Read voltage and current from power supply",
        .category = CommandCategory::Power,
        .parameters = {
            {
                .name = "channel",
                .displayName = "Channel",
                .description = "Power supply channel",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = false,
                .minValue = 1,
                .maxValue = 4
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            int channel = params.value("channel", 1).toInt();
            qDebug() << "Reading measurements from channel" << channel;
            QVariantMap response;
            response["voltage"] = 12.05;
            response["current"] = 1.23;
            response["power"] = 14.82;
            return CommandResult::Success("Measurements read", response);
        }
    });
}

//=============================================================================
// Flow Commands Registration
//=============================================================================

void CommandRegistry::registerFlowCommands()
{
    // Wait
    registerCommand({
        .id = "flow_wait",
        .name = "Wait",
        .description = "Pause execution for specified time",
        .category = CommandCategory::Flow,
        .parameters = {
            {
                .name = "duration_ms",
                .displayName = "Duration",
                .description = "Time to wait",
                .type = ParameterType::Duration,
                .defaultValue = 1000,
                .required = true,
                .minValue = 0,
                .maxValue = 300000,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            int durationMs = params.value("duration_ms", 1000).toInt();
            qDebug() << "Waiting for" << durationMs << "ms";
            QThread::msleep(durationMs);
            return CommandResult::Success(QString("Waited %1ms").arg(durationMs));
        }
    });
    
    // Repeat
    registerCommand({
        .id = "flow_repeat",
        .name = "Repeat Previous Step",
        .description = "Repeat the previous step N times",
        .category = CommandCategory::Flow,
        .parameters = {
            {
                .name = "count",
                .displayName = "Repeat Count",
                .description = "Number of times to repeat",
                .type = ParameterType::Integer,
                .defaultValue = 1,
                .required = true,
                .minValue = 1,
                .maxValue = 1000
            },
            {
                .name = "delay_ms",
                .displayName = "Delay Between",
                .description = "Delay between repetitions",
                .type = ParameterType::Duration,
                .defaultValue = 100,
                .required = false,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            int count = params.value("count", 1).toInt();
            qDebug() << "Repeat flag set:" << count << "times";
            return CommandResult::Success(QString("Will repeat %1 times").arg(count));
        }
    });
    
    // Log Message
    registerCommand({
        .id = "flow_log",
        .name = "Log Message",
        .description = "Write a message to the test log",
        .category = CommandCategory::Flow,
        .parameters = {
            {
                .name = "message",
                .displayName = "Message",
                .description = "Message to log",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "level",
                .displayName = "Log Level",
                .description = "Log severity level",
                .type = ParameterType::Enum,
                .defaultValue = "INFO",
                .required = false,
                .enumValues = {"DEBUG", "INFO", "WARNING", "ERROR"}
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString message = params.value("message").toString();
            QString level = params.value("level", "INFO").toString();
            qDebug() << "[" << level << "]" << message;
            return CommandResult::Success("Logged message");
        }
    });
    
    // Set Variable
    registerCommand({
        .id = "flow_set_variable",
        .name = "Set Variable",
        .description = "Set a variable value for use in subsequent steps",
        .category = CommandCategory::Flow,
        .parameters = {
            {
                .name = "variable_name",
                .displayName = "Variable Name",
                .description = "Name of the variable",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "value",
                .displayName = "Value",
                .description = "Value to assign",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString name = params.value("variable_name").toString();
            QString value = params.value("value").toString();
            qDebug() << "Set variable" << name << "=" << value;
            QVariantMap response;
            response[name] = value;
            return CommandResult::Success("Variable set", response);
        }
    });
}

//=============================================================================
// Validation Commands Registration
//=============================================================================

void CommandRegistry::registerValidationCommands()
{
    // Assert Equals
    registerCommand({
        .id = "validate_equals",
        .name = "Assert Equals",
        .description = "Assert that a value equals expected",
        .category = CommandCategory::Validation,
        .parameters = {
            {
                .name = "actual",
                .displayName = "Actual Value",
                .description = "The actual value to check",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "expected",
                .displayName = "Expected Value",
                .description = "The expected value",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "message",
                .displayName = "Error Message",
                .description = "Message to show on failure",
                .type = ParameterType::String,
                .defaultValue = "Values do not match",
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString actual = params.value("actual").toString();
            QString expected = params.value("expected").toString();
            QString message = params.value("message", "Values do not match").toString();
            
            if (actual == expected) {
                return CommandResult::Success("Assertion passed: " + actual + " == " + expected);
            } else {
                return CommandResult::Failure(message + ": Expected '" + expected + "' but got '" + actual + "'");
            }
        }
    });
    
    // Assert Contains
    registerCommand({
        .id = "validate_contains",
        .name = "Assert Contains",
        .description = "Assert that a string contains expected substring",
        .category = CommandCategory::Validation,
        .parameters = {
            {
                .name = "haystack",
                .displayName = "String to Search",
                .description = "The string to search in",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "needle",
                .displayName = "Substring",
                .description = "The substring to find",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString haystack = params.value("haystack").toString();
            QString needle = params.value("needle").toString();
            
            if (haystack.contains(needle)) {
                return CommandResult::Success("String contains '" + needle + "'");
            } else {
                return CommandResult::Failure("String does not contain '" + needle + "'");
            }
        }
    });
    
    // Assert Regex Match
    registerCommand({
        .id = "validate_regex",
        .name = "Assert Regex Match",
        .description = "Assert that a string matches a regular expression",
        .category = CommandCategory::Validation,
        .parameters = {
            {
                .name = "text",
                .displayName = "Text",
                .description = "The text to match against",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "pattern",
                .displayName = "Regex Pattern",
                .description = "Regular expression pattern",
                .type = ParameterType::String,
                .defaultValue = ".*",
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString text = params.value("text").toString();
            QString pattern = params.value("pattern").toString();
            
            QRegularExpression regex(pattern);
            if (!regex.isValid()) {
                return CommandResult::Failure("Invalid regex pattern: " + regex.errorString());
            }
            if (regex.match(text).hasMatch()) {
                return CommandResult::Success("Regex match successful");
            } else {
                return CommandResult::Failure("Regex pattern '" + pattern + "' did not match");
            }
        }
    });
    
    // Assert Numeric Range
    registerCommand({
        .id = "validate_range",
        .name = "Assert In Range",
        .description = "Assert that a numeric value is within a range",
        .category = CommandCategory::Validation,
        .parameters = {
            {
                .name = "value",
                .displayName = "Value",
                .description = "The numeric value to check",
                .type = ParameterType::Double,
                .defaultValue = 0.0,
                .required = true
            },
            {
                .name = "min",
                .displayName = "Minimum",
                .description = "Minimum allowed value",
                .type = ParameterType::Double,
                .defaultValue = 0.0,
                .required = true
            },
            {
                .name = "max",
                .displayName = "Maximum",
                .description = "Maximum allowed value",
                .type = ParameterType::Double,
                .defaultValue = 100.0,
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            double value = params.value("value").toDouble();
            double min = params.value("min").toDouble();
            double max = params.value("max").toDouble();
            
            if (value >= min && value <= max) {
                return CommandResult::Success(QString("Value %1 is within range [%2, %3]").arg(value).arg(min).arg(max));
            } else {
                return CommandResult::Failure(QString("Value %1 is outside range [%2, %3]").arg(value).arg(min).arg(max));
            }
        }
    });
}

//=============================================================================
// System Commands Registration
//=============================================================================

void CommandRegistry::registerSystemCommands()
{
    // Screenshot
    registerCommand({
        .id = "system_screenshot",
        .name = "Take Screenshot",
        .description = "Capture a screenshot and save to file",
        .category = CommandCategory::System,
        .parameters = {
            {
                .name = "filename",
                .displayName = "Filename",
                .description = "Output filename (without extension)",
                .type = ParameterType::String,
                .defaultValue = "screenshot",
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString filename = params.value("filename", "screenshot").toString();
            qDebug() << "Taking screenshot:" << filename;
            // TODO: Implement actual screenshot
            return CommandResult::Success("Screenshot saved: " + filename + ".png");
        }
    });
    
    // Save to File
    registerCommand({
        .id = "system_save_file",
        .name = "Save to File",
        .description = "Save last response data to a file",
        .category = CommandCategory::System,
        .parameters = {
            {
                .name = "filepath",
                .displayName = "File Path",
                .description = "Path to save the file",
                .type = ParameterType::FilePath,
                .defaultValue = "",
                .required = true
            },
            {
                .name = "data",
                .displayName = "Data",
                .description = "Data to save (or use ${last_response})",
                .type = ParameterType::String,
                .defaultValue = "${last_response}",
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString filepath = params.value("filepath").toString();
            QString data = params.value("data", "${last_response}").toString();
            qDebug() << "Saving data to:" << filepath;
            // TODO: Implement actual file save
            return CommandResult::Success("Data saved to: " + filepath);
        }
    });
    
    // Add Comment
    registerCommand({
        .id = "system_comment",
        .name = "Add Comment",
        .description = "Add a comment to the test report",
        .category = CommandCategory::System,
        .parameters = {
            {
                .name = "comment",
                .displayName = "Comment",
                .description = "Comment text to add to report",
                .type = ParameterType::String,
                .defaultValue = "",
                .required = true
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/, const std::atomic<bool>* /*cancel*/) -> CommandResult {
            QString comment = params.value("comment").toString();
            qDebug() << "Comment:" << comment;
            return CommandResult::Success("Comment added");
        }
    });
}

} // namespace TestExecutor
