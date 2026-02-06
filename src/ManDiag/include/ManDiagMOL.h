#pragma once
/**
 * @file ManDiagMOL.h
 * @brief Manufacturing Diagnostics Mid-of-Line (MOL) Command Types.
 *
 * MOL commands work with the CAN interface. This module is a placeholder
 * for future implementation.
 *
 * MOL commands will include similar types to EOL:
 * - MD_Tx: Send CAN message without waiting for response
 * - MD_TxRx: Send CAN message and match response
 * - MD_Type1: Send, receive, parse, and validate CAN response
 *
 * @note This file is a placeholder for future implementation.
 *       CAN interface support will be added in a future release.
 */

#include "ManDiagProtocol.h"
#include <QObject>
#include <QString>
#include <QVariantMap>

namespace ManDiag {
namespace MOL {

//=============================================================================
// MOL Configuration (Placeholder)
//=============================================================================

/**
 * @brief Configuration for MOL CAN commands
 */
struct MOLConfig {
    QString canInterface;           ///< CAN interface name (e.g., "can0")
    uint32_t txCanId = 0x7DF;       ///< Transmit CAN ID
    uint32_t rxCanId = 0x7E8;       ///< Receive CAN ID (expected response ID)
    int timeoutMs = 5000;           ///< Command timeout
    int retries = 1;                ///< Number of retry attempts
};

//=============================================================================
// MOL Command Result (Placeholder)
//=============================================================================

/**
 * @brief Result of a MOL command execution
 */
struct MOLResult {
    bool success = false;           ///< Whether command succeeded
    QString message;                ///< Result or error message
    ManDiagResponse response;       ///< Parsed response
    QString rawResponse;            ///< Raw response data
    
    /**
     * @brief Convert to QVariantMap for command registry integration
     */
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["success"] = success;
        map["message"] = message;
        map["rawResponse"] = rawResponse;
        
        if (response.valid) {
            map["parsedResponse"] = response.toVariantMap();
        }
        
        return map;
    }
    
    static MOLResult NotImplemented() {
        MOLResult r;
        r.success = false;
        r.message = "MOL commands are not yet implemented. CAN interface support coming soon.";
        return r;
    }
};

//=============================================================================
// MOL Commands (Placeholders)
//=============================================================================

/**
 * @brief MD_Tx for CAN: Send command without waiting for response.
 * @note Not yet implemented
 */
inline MOLResult MD_Tx(const QString& /*command*/, const MOLConfig& /*config*/)
{
    return MOLResult::NotImplemented();
}

/**
 * @brief MD_TxRx for CAN: Send command and match response.
 * @note Not yet implemented
 */
inline MOLResult MD_TxRx(const QString& /*command*/,
                          const QString& /*expectedResponse*/,
                          const MOLConfig& /*config*/)
{
    return MOLResult::NotImplemented();
}

/**
 * @brief MD_Type1 for CAN: Send, parse, and validate response.
 * @note Not yet implemented
 */
inline MOLResult MD_Type1(const QString& /*command*/,
                           const ValidationOptions& /*validation*/,
                           const MOLConfig& /*config*/)
{
    return MOLResult::NotImplemented();
}

//=============================================================================
// MOL Command Registry Integration (Placeholder)
//=============================================================================

/**
 * @brief Register MOL commands with the CommandRegistry.
 * @note Commands are registered but return "not implemented" until CAN support is added.
 */
void registerMOLCommands();

} // namespace MOL
} // namespace ManDiag
