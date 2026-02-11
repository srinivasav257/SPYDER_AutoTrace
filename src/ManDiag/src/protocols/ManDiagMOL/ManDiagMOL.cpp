/**
 * @file ManDiagMOL.cpp
 * @brief ManDiag MOL protocol scaffold.
 */

#include "protocols/ManDiagMOL/ManDiagMOL.h"
#include "CommandRegistry.h"
#include <QDebug>

namespace ManDiag::MOL {

void registerMOLCommands()
{
    using namespace TestExecutor;
    auto& registry = CommandRegistry::instance();

    registry.registerCommand({
        .id = "mandiag_mol_placeholder",
        .name = "MD_MOL_Placeholder",
        .description = "Placeholder command for ManDiagMOL protocol scaffold.",
        .category = CommandCategory::ManDiagMOL,
        .parameters = {},
        .handler = [](const QVariantMap& /*params*/,
                      const QVariantMap& /*config*/,
                      const std::atomic<bool>* /*cancel*/) -> CommandResult {
            return CommandResult::Failure("ManDiagMOL is scaffolded but not implemented yet.");
        }
    });

    qDebug() << "ManDiag MOL commands registered (placeholder)";
}

} // namespace ManDiag::MOL

