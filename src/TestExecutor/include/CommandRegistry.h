#pragma once
/**
 * @file CommandRegistry.h
 * @brief Registry of predefined test commands.
 * 
 * The CommandRegistry provides the "no-code" functionality:
 * - Defines all available test commands
 * - Each command has defined input parameters with types
 * - Each command has a handler function that executes it
 * 
 * Users simply select a command and fill in parameters - no coding required!
 */

#include "TestDataModels.h"
#include <QObject>
#include <QVariant>
#include <functional>
#include <QMap>

namespace TestExecutor {

//=============================================================================
// Parameter Types
//=============================================================================

/**
 * @brief Type of parameter for UI editing
 */
enum class ParameterType {
    String,         ///< Text input
    Integer,        ///< Integer spinner
    Double,         ///< Double spinner
    Boolean,        ///< Checkbox
    Enum,           ///< Dropdown selection
    HexString,      ///< Hex data input (e.g., "6D 64 3E")
    FilePath,       ///< File browser
    Duration,       ///< Duration input (ms)
    CanId,          ///< CAN ID input (hex)
    ComPort,        ///< COM port selector
    Voltage,        ///< Voltage input
    Current         ///< Current input
};

/**
 * @brief Definition of a command parameter
 */
struct ParameterDef
{
    QString name;               ///< Parameter name (key in QVariantMap)
    QString displayName;        ///< Human-readable name
    QString description;        ///< Help text
    ParameterType type;         ///< Parameter type
    QVariant defaultValue;      ///< Default value
    bool required = true;       ///< Whether parameter is required
    
    // For Enum type
    QStringList enumValues;     ///< List of allowed values
    
    // For numeric types
    QVariant minValue;          ///< Minimum value
    QVariant maxValue;          ///< Maximum value
    QString unit;               ///< Unit suffix (e.g., "ms", "V")
};

//=============================================================================
// Command Result
//=============================================================================

/**
 * @brief Result returned by command execution
 */
struct CommandResult
{
    bool success = false;           ///< Whether command succeeded
    QString message;                ///< Result or error message
    QVariantMap responseData;       ///< Data returned by command
    
    static CommandResult Success(const QString& msg = "OK", const QVariantMap& data = {}) {
        return {true, msg, data};
    }
    
    static CommandResult Failure(const QString& msg) {
        return {false, msg, {}};
    }
};

//=============================================================================
// Command Handler
//=============================================================================

/**
 * @brief Function signature for command handlers
 * @param params Input parameters
 * @param config Global configuration
 * @return Execution result
 */
using CommandHandler = std::function<CommandResult(const QVariantMap& params, const QVariantMap& config)>;

//=============================================================================
// Command Definition
//=============================================================================

/**
 * @brief Complete definition of a test command
 */
struct CommandDef
{
    QString id;                     ///< Unique command ID
    QString name;                   ///< Display name
    QString description;            ///< What the command does
    CommandCategory category;       ///< Category for grouping
    QVector<ParameterDef> parameters; ///< Required and optional parameters
    CommandHandler handler;         ///< Function that executes the command
    
    bool isValid() const { return !id.isEmpty() && handler != nullptr; }
};

//=============================================================================
// CommandRegistry Singleton
//=============================================================================

/**
 * @brief Central registry of all available test commands.
 * 
 * Usage:
 * @code
 * auto& reg = CommandRegistry::instance();
 * 
 * // Get all commands in a category
 * auto serialCmds = reg.commandsByCategory(CommandCategory::Serial);
 * 
 * // Execute a command
 * QVariantMap params = {{"hex_command", "6D 64 3E"}, {"timeout_ms", 5000}};
 * auto result = reg.execute("send_mandiag_command", params, globalConfig);
 * @endcode
 */
class CommandRegistry : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     */
    static CommandRegistry& instance();

    // === Registration ===
    
    /**
     * @brief Register a new command
     * @return true if registered successfully
     */
    bool registerCommand(const CommandDef& cmd);
    bool registerCommand(CommandDef&& cmd);
    
    /**
     * @brief Register all built-in commands
     * Call this at application startup.
     */
    void registerBuiltinCommands();

    // === Access ===
    
    /**
     * @brief Get command by ID
     */
    const CommandDef* command(const QString& id) const;
    
    /**
     * @brief Get all commands
     */
    QList<CommandDef> allCommands() const { return m_commands.values(); }
    
    /**
     * @brief Get commands by category
     */
    QList<CommandDef> commandsByCategory(CommandCategory category) const;
    
    /**
     * @brief Get all unique categories
     */
    QList<CommandCategory> categories() const;
    
    /**
     * @brief Get command names for a category (for UI combo boxes)
     */
    QStringList commandNamesForCategory(CommandCategory category) const;

    // === Execution ===
    
    /**
     * @brief Execute a command
     * @param commandId Command to execute
     * @param params Parameters for the command
     * @param config Global configuration
     * @return Execution result
     */
    CommandResult execute(const QString& commandId, 
                          const QVariantMap& params,
                          const QVariantMap& config);
    
    /**
     * @brief Validate parameters for a command
     * @return Empty string if valid, error message if invalid
     */
    QString validateParameters(const QString& commandId, const QVariantMap& params) const;

private:
    CommandRegistry();
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;

    void registerSerialCommands();
    void registerCANCommands();
    void registerPowerCommands();
    void registerFlowCommands();
    void registerValidationCommands();
    void registerSystemCommands();

    QMap<QString, CommandDef> m_commands;
};

//=============================================================================
// Utility Functions
//=============================================================================

/**
 * @brief Convert hex string to byte array
 * @param hex String like "6D 64 3E" or "6D643E"
 */
QByteArray hexStringToBytes(const QString& hex);

/**
 * @brief Convert byte array to hex string
 * @param bytes Raw bytes
 * @param separator Separator between bytes (default: space)
 */
QString bytesToHexString(const QByteArray& bytes, const QString& separator = " ");

} // namespace TestExecutor
