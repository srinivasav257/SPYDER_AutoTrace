#pragma once
/**
 * @file ManDiag.h
 * @brief Manufacturing Diagnostics Module - Main Header
 *
 * This header provides the main interface for the ManDiag module,
 * including initialization and command registration.
 *
 * Usage:
 * @code
 * #include <ManDiag.h>
 *
 * // After TestExecutorEngine is initialized
 * ManDiag::registerAllCommands();
 * @endcode
 *
 * Command Categories:
 * - EOL (End of Line): Serial interface commands
 * - MOL (Mid of Line): CAN interface commands (placeholder)
 *
 * Command Types:
 * - MDU_Tx: Send command only via UART (fire and forget)
 * - MDU_TxRx: Send via UART and match entire response
 * - MDU_Type1: Send via UART, parse, and validate individual fields
 */

#include "ManDiagProtocol.h"
#include "ManDiagEOL.h"
#include "ManDiagMOL.h"

namespace ManDiag {

/**
 * @brief Register all ManDiag commands with the CommandRegistry.
 *
 * This function registers both EOL and MOL commands.
 * Call this after TestExecutorEngine is initialized.
 */
inline void registerAllCommands()
{
    EOL::registerEOLCommands();
    MOL::registerMOLCommands();
}

} // namespace ManDiag
