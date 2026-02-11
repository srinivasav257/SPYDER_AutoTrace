/**
 * @file ManDiagPITS.cpp
 * @brief ManDiag PITS protocol scaffold.
 */

#include "protocols/ManDiagPITS/ManDiagPITS.h"
#include "CommandRegistry.h"
#include <QDebug>

namespace ManDiag::PITS {

void registerPITSCommands()
{
    using namespace TestExecutor;
    auto& registry = CommandRegistry::instance();

    registry.registerCommand({
        .id = "mandiag_pits_placeholder",
        .name = "MD_PITS_Placeholder",
        .description = "Placeholder command for ManDiagPITS protocol scaffold.",
        .category = CommandCategory::ManDiagPITS,
        .parameters = {},
        .handler = [](const QVariantMap& /*params*/,
                      const QVariantMap& /*config*/,
                      const std::atomic<bool>* /*cancel*/) -> CommandResult {
            return CommandResult::Failure("ManDiagPITS is scaffolded but not implemented yet.");
        }
    });

    qDebug() << "ManDiag PITS commands registered (placeholder)";
}

} // namespace ManDiag::PITS

