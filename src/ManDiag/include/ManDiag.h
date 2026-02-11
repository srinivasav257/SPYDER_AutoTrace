#pragma once
/**
 * @file ManDiag.h
 * @brief Manufacturing Diagnostics Module - Main Header
 *
 * This header provides the main interface for ManDiag command registration.
 *
 * Protocols:
 * - ITS  (implemented first)
 * - PITS (scaffold placeholder)
 * - MOL  (scaffold placeholder)
 */

#include "protocols/ManDiagITS/ManDiagITS.h"
#include "protocols/ManDiagPITS/ManDiagPITS.h"
#include "protocols/ManDiagMOL/ManDiagMOL.h"

namespace ManDiag {

/**
 * @brief Register all ManDiag commands with the CommandRegistry.
 *
 * This function registers ITS, PITS, and MOL protocol command sets.
 * Call this after TestExecutorEngine is initialized.
 */
inline void registerAllCommands()
{
    ITS::registerITSCommands();
    PITS::registerPITSCommands();
    MOL::registerMOLCommands();
}

} // namespace ManDiag
