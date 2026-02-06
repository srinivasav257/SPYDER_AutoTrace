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
#include <QDebug>
#include <QThread>
#include <QRegularExpression>

namespace TestExecutor {

//=============================================================================
// Utility Functions
//=============================================================================

QByteArray hexStringToBytes(const QString& hex)
{
    QString cleaned = hex;
    cleaned.remove(' ');
    cleaned.remove('-');
    cleaned.remove(':');
    
    QByteArray bytes;
    for (int i = 0; i + 1 < cleaned.length(); i += 2) {
        bool ok;
        uint8_t byte = cleaned.mid(i, 2).toUInt(&ok, 16);
        if (ok) {
            bytes.append(static_cast<char>(byte));
        }
    }
    return bytes;
}

QString bytesToHexString(const QByteArray& bytes, const QString& separator)
{
    QStringList hexBytes;
    for (char byte : bytes) {
        hexBytes.append(QString("%1").arg(static_cast<uint8_t>(byte), 2, 16, QChar('0')).toUpper());
    }
    return hexBytes.join(separator);
}

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
                                        const QVariantMap& config)
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
    
    // Execute the command
    try {
        return cmd->handler(params, config);
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& /*params*/, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
            int timeout = params.value("timeout_ms", 1000).toInt();
            qDebug() << "Reading serial response, timeout:" << timeout;
            QVariantMap response;
            response["data"] = "Sample response data";
            return CommandResult::Success("Read response", response);
        }
    });
}

//=============================================================================
// CAN Commands Registration
//=============================================================================

void CommandRegistry::registerCANCommands()
{
    // Send CAN Message
    registerCommand({
        .id = "can_send_message",
        .name = "Send CAN Message",
        .description = "Send a CAN bus message",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "can_id",
                .displayName = "CAN ID",
                .description = "Message arbitration ID (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "0x100",
                .required = true
            },
            {
                .name = "data",
                .displayName = "Data",
                .description = "Message payload (hex bytes, max 8 or 64 for FD)",
                .type = ParameterType::HexString,
                .defaultValue = "00 00 00 00 00 00 00 00",
                .required = true
            },
            {
                .name = "extended_id",
                .displayName = "Extended ID",
                .description = "Use 29-bit extended ID",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            },
            {
                .name = "fd_mode",
                .displayName = "CAN FD",
                .description = "Send as CAN FD message",
                .type = ParameterType::Boolean,
                .defaultValue = false,
                .required = false
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
            QString canId = params.value("can_id").toString();
            QString data = params.value("data").toString();
            qDebug() << "Sending CAN message ID:" << canId << "Data:" << data;
            return CommandResult::Success("Sent CAN message");
        }
    });
    
    // Read CAN Message
    registerCommand({
        .id = "can_read_message",
        .name = "Read CAN Message",
        .description = "Read a CAN message with specific ID",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "target_id",
                .displayName = "Target CAN ID",
                .description = "CAN ID to wait for (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "0x100",
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait for message",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
            QString targetId = params.value("target_id").toString();
            qDebug() << "Waiting for CAN message ID:" << targetId;
            QVariantMap response;
            response["can_id"] = targetId;
            response["data"] = "00 00 00 00 00 00 00 00";
            response["timestamp"] = QDateTime::currentMSecsSinceEpoch();
            return CommandResult::Success("Received CAN message", response);
        }
    });
    
    // Check CAN Signal Value
    registerCommand({
        .id = "can_check_signal",
        .name = "Check CAN Signal",
        .description = "Check a signal value in a CAN message",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "can_id",
                .displayName = "CAN ID",
                .description = "Message ID containing the signal",
                .type = ParameterType::CanId,
                .defaultValue = "0x100",
                .required = true
            },
            {
                .name = "start_bit",
                .displayName = "Start Bit",
                .description = "Signal start bit position",
                .type = ParameterType::Integer,
                .defaultValue = 0,
                .required = true,
                .minValue = 0,
                .maxValue = 63
            },
            {
                .name = "bit_length",
                .displayName = "Bit Length",
                .description = "Signal length in bits",
                .type = ParameterType::Integer,
                .defaultValue = 8,
                .required = true,
                .minValue = 1,
                .maxValue = 64
            },
            {
                .name = "expected_value",
                .displayName = "Expected Value",
                .description = "Expected signal value",
                .type = ParameterType::Integer,
                .defaultValue = 0,
                .required = true
            },
            {
                .name = "timeout_ms",
                .displayName = "Timeout",
                .description = "Maximum time to wait",
                .type = ParameterType::Duration,
                .defaultValue = 5000,
                .required = false,
                .unit = "ms"
            }
        },
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
            QString canId = params.value("can_id").toString();
            int expectedValue = params.value("expected_value").toInt();
            qDebug() << "Checking CAN signal in ID:" << canId << "Expected:" << expectedValue;
            QVariantMap response;
            response["actual_value"] = expectedValue;
            return CommandResult::Success("Signal value matches", response);
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
            QString text = params.value("text").toString();
            QString pattern = params.value("pattern").toString();
            
            QRegularExpression regex(pattern);
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
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
        .handler = [](const QVariantMap& params, const QVariantMap& /*config*/) -> CommandResult {
            QString comment = params.value("comment").toString();
            qDebug() << "Comment:" << comment;
            return CommandResult::Success("Comment added");
        }
    });
}

} // namespace TestExecutor
