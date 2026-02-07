/**
 * @file tst_CommandRegistry.cpp
 * @brief Unit tests for CommandRegistry — registration, lookup, validation,
 *        and execution of commands (no real hardware needed).
 */

#include <gtest/gtest.h>
#include "CommandRegistry.h"
#include <QCoreApplication>
#include <atomic>

using namespace TestExecutor;

// ============================================================================
// Fixture — initialise singleton once
// ============================================================================

class CommandRegistryTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        // Ensure QCoreApplication exists (required by Qt signal/slot)
        if (!QCoreApplication::instance()) {
            static int argc = 1;
            static char arg0[] = "test";
            static char* argv[] = {arg0, nullptr};
            static QCoreApplication app(argc, argv);
        }

        auto& reg = CommandRegistry::instance();
        reg.registerBuiltinCommands();
    }

    CommandRegistry& reg() { return CommandRegistry::instance(); }
};

// ============================================================================
// Registration & lookup
// ============================================================================

TEST_F(CommandRegistryTest, BuiltinCommandsRegistered)
{
    auto all = reg().allCommands();
    EXPECT_GT(all.size(), 10) << "Expected many built-in commands";
}

TEST_F(CommandRegistryTest, LookupKnownCommand)
{
    const CommandDef* cmd = reg().command("flow_wait");
    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->name, "Wait");
    EXPECT_EQ(cmd->category, CommandCategory::Flow);
}

TEST_F(CommandRegistryTest, LookupUnknownCommandReturnsNull)
{
    const CommandDef* cmd = reg().command("nonexistent_command_xyz");
    EXPECT_EQ(cmd, nullptr);
}

TEST_F(CommandRegistryTest, CommandsByCategory)
{
    auto serialCmds = reg().commandsByCategory(CommandCategory::Serial);
    EXPECT_GT(serialCmds.size(), 0);
    for (const auto& cmd : serialCmds) {
        EXPECT_EQ(cmd.category, CommandCategory::Serial);
    }
}

TEST_F(CommandRegistryTest, CategoriesNotEmpty)
{
    auto cats = reg().categories();
    EXPECT_GT(cats.size(), 0);
}

TEST_F(CommandRegistryTest, CommandNamesForCategory)
{
    auto names = reg().commandNamesForCategory(CommandCategory::Flow);
    EXPECT_GT(names.size(), 0);
    // "Wait" should be among them
    EXPECT_TRUE(names.contains("Wait") || names.contains("flow_wait"));
}

// ============================================================================
// Parameter validation
// ============================================================================

TEST_F(CommandRegistryTest, ValidateParameters_MissingRequired)
{
    // serial_enter_md_session requires 'session_type'
    QVariantMap empty;
    QString error = reg().validateParameters("serial_enter_md_session", empty);
    EXPECT_FALSE(error.isEmpty()) << "Missing required param should produce error";
}

TEST_F(CommandRegistryTest, ValidateParameters_AllPresent)
{
    QVariantMap params;
    params["session_type"] = "manufacturing";
    QString error = reg().validateParameters("serial_enter_md_session", params);
    EXPECT_TRUE(error.isEmpty()) << "Valid params should pass: " << error.toStdString();
}

TEST_F(CommandRegistryTest, ValidateParameters_UnknownCommand)
{
    QString error = reg().validateParameters("nonexistent_xyz", {});
    EXPECT_FALSE(error.isEmpty());
}

// ============================================================================
// Execution — stub commands return Failure("Not implemented")
// ============================================================================

TEST_F(CommandRegistryTest, StubCommand_ReturnNotImplemented)
{
    QVariantMap params;
    params["session_type"] = "manufacturing";
    auto result = reg().execute("serial_enter_md_session", params, {});

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.message.contains("Not implemented"))
        << "Stub should say 'Not implemented', got: " << result.message.toStdString();
}

TEST_F(CommandRegistryTest, StubReadDID_ReturnNotImplemented)
{
    QVariantMap params;
    params["did_id"] = "F190";
    auto result = reg().execute("serial_read_did", params, {});

    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.message.contains("Not implemented"));
}

TEST_F(CommandRegistryTest, StubPowerOn_ReturnNotImplemented)
{
    auto result = reg().execute("power_turn_on", {}, {});
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.message.contains("Not implemented"));
}

// ============================================================================
// Execution — flow_wait actually works
// ============================================================================

TEST_F(CommandRegistryTest, FlowWait_Succeeds)
{
    QVariantMap params;
    params["duration_ms"] = 10; // very short wait for tests
    auto result = reg().execute("flow_wait", params, {});
    EXPECT_TRUE(result.success);
}

// ============================================================================
// Execution with cancellation flag
// ============================================================================

TEST_F(CommandRegistryTest, ExecuteUnknownCommand_Fails)
{
    auto result = reg().execute("nonexistent_command", {}, {});
    EXPECT_FALSE(result.success);
}

// ============================================================================
// Custom command registration
// ============================================================================

TEST_F(CommandRegistryTest, RegisterCustomCommand)
{
    CommandDef custom;
    custom.id = "test_custom_cmd";
    custom.name = "Custom Command";
    custom.description = "A test-only command";
    custom.category = CommandCategory::System;
    custom.handler = [](const QVariantMap&, const QVariantMap&, const std::atomic<bool>*) -> CommandResult {
        return CommandResult::Success("custom OK");
    };

    bool ok = reg().registerCommand(custom);
    EXPECT_TRUE(ok);

    const CommandDef* found = reg().command("test_custom_cmd");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Custom Command");

    auto result = reg().execute("test_custom_cmd", {}, {});
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "custom OK");
}
