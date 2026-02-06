/**
 * @file ManDiagMOL.cpp
 * @brief Implementation of MOL (Mid-of-Line) ManDiag commands.
 *
 * This file provides placeholder implementations for MOL commands.
 * Full CAN interface support will be added in a future release.
 */

#include "ManDiagMOL.h"
#include "ManDiagProtocol.h"
#include "CommandRegistry.h"
#include <QDebug>

namespace ManDiag {
namespace MOL {

//=============================================================================
// Command Registry Integration
//=============================================================================

void registerMOLCommands()
{
    using namespace TestExecutor;
    auto& registry = CommandRegistry::instance();
    
    // =========================================================================
    // MD_Tx CAN - Transmit Only (Placeholder)
    // =========================================================================
    registry.registerCommand({
        .id = "mandiag_mol_tx",
        .name = "MD_Tx CAN (Send Only)",
        .description = "[PLACEHOLDER] Send ManDiag command via CAN without waiting for response. "
                       "CAN interface support is not yet implemented.",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "can_data",
                .displayName = "CAN Data (Hex)",
                .description = "CAN message data bytes (hex)",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "can_id",
                .displayName = "CAN ID",
                .description = "Transmit CAN ID (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "7DF",
                .required = true
            },
            {
                .name = "can_interface",
                .displayName = "CAN Interface",
                .description = "CAN interface name",
                .type = ParameterType::String,
                .defaultValue = "can0",
                .required = true
            }
        },
        .handler = [](const QVariantMap& /*params*/, const QVariantMap& /*config*/) -> CommandResult {
            return CommandResult::Failure("MOL commands are not yet implemented. CAN interface support coming soon.");
        }
    });
    
    // =========================================================================
    // MD_TxRx CAN - Transmit and Match Response (Placeholder)
    // =========================================================================
    registry.registerCommand({
        .id = "mandiag_mol_txrx",
        .name = "MD_TxRx CAN (Send & Match)",
        .description = "[PLACEHOLDER] Send ManDiag command via CAN and match response. "
                       "CAN interface support is not yet implemented.",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "can_data",
                .displayName = "CAN Data (Hex)",
                .description = "CAN message data bytes to send (hex)",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "expected_response",
                .displayName = "Expected Response",
                .description = "Expected CAN response data (hex)",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3C 00 01 01 01 00",
                .required = true
            },
            {
                .name = "tx_can_id",
                .displayName = "TX CAN ID",
                .description = "Transmit CAN ID (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "7DF",
                .required = true
            },
            {
                .name = "rx_can_id",
                .displayName = "RX CAN ID",
                .description = "Expected receive CAN ID (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "7E8",
                .required = true
            },
            {
                .name = "can_interface",
                .displayName = "CAN Interface",
                .description = "CAN interface name",
                .type = ParameterType::String,
                .defaultValue = "can0",
                .required = true
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
        .handler = [](const QVariantMap& /*params*/, const QVariantMap& /*config*/) -> CommandResult {
            return CommandResult::Failure("MOL commands are not yet implemented. CAN interface support coming soon.");
        }
    });
    
    // =========================================================================
    // MD_Type1 CAN - Parse and Validate (Placeholder)
    // =========================================================================
    registry.registerCommand({
        .id = "mandiag_mol_type1",
        .name = "MD_Type1 CAN (Parse & Validate)",
        .description = "[PLACEHOLDER] Send ManDiag command via CAN, parse response, and validate fields. "
                       "CAN interface support is not yet implemented.",
        .category = CommandCategory::CAN,
        .parameters = {
            {
                .name = "can_data",
                .displayName = "CAN Data (Hex)",
                .description = "CAN message data bytes to send (hex)",
                .type = ParameterType::HexString,
                .defaultValue = "6D 64 3E 00 01 01 00 01 01",
                .required = true
            },
            {
                .name = "validate_status",
                .displayName = "Validate Status",
                .description = "Check if status byte matches expected value",
                .type = ParameterType::Boolean,
                .defaultValue = true,
                .required = false
            },
            {
                .name = "expected_status",
                .displayName = "Expected Status",
                .description = "Expected status byte (hex)",
                .type = ParameterType::HexString,
                .defaultValue = "01",
                .required = false
            },
            {
                .name = "tx_can_id",
                .displayName = "TX CAN ID",
                .description = "Transmit CAN ID (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "7DF",
                .required = true
            },
            {
                .name = "rx_can_id",
                .displayName = "RX CAN ID",
                .description = "Expected receive CAN ID (hex)",
                .type = ParameterType::CanId,
                .defaultValue = "7E8",
                .required = true
            },
            {
                .name = "can_interface",
                .displayName = "CAN Interface",
                .description = "CAN interface name",
                .type = ParameterType::String,
                .defaultValue = "can0",
                .required = true
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
        .handler = [](const QVariantMap& /*params*/, const QVariantMap& /*config*/) -> CommandResult {
            return CommandResult::Failure("MOL commands are not yet implemented. CAN interface support coming soon.");
        }
    });
    
    qDebug() << "ManDiag MOL commands registered (placeholders)";
}

} // namespace MOL
} // namespace ManDiag
