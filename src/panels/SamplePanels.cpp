#include "SamplePanels.h"

#include "IconManager.h"
#include "PanelRegistry.h"
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

namespace {

QWidget* createPlaceholderPanel(const QString& title, QWidget* parent)
{
    auto* root = new QWidget(parent);
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* label = new QLabel(
        QObject::tr("%1\n\nPlaceholder panel.\nFunctional widget will be added in a future step.").arg(title),
        root);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);

    layout->addStretch(1);
    layout->addWidget(label);
    layout->addStretch(1);
    return root;
}

} // namespace

namespace SamplePanels {

bool registerSamplePanels()
{
    auto& registry = DockManager::PanelRegistry::instance();
    bool success = true;

    success &= registry.registerPanel({
        .id = "can_trace",
        .title = "CAN Trace",
        .category = "CANalyzer",
        .defaultArea = ads::CenterDockWidgetArea,
        .factory = [](QWidget* parent) -> QWidget* {
            return createPlaceholderPanel(QObject::tr("CAN Trace"), parent);
        },
        .icon = DockManager::Icons::icon(DockManager::Icons::Id::ActivityCanalyzer)
    });

    success &= registry.registerPanel({
        .id = "ig_block",
        .title = "IG Block",
        .category = "CANalyzer",
        .defaultArea = ads::RightDockWidgetArea,
        .factory = [](QWidget* parent) -> QWidget* {
            return createPlaceholderPanel(QObject::tr("IG Block"), parent);
        },
        .icon = DockManager::Icons::icon(DockManager::Icons::Id::ActivitySettings)
    });

    return success;
}

} // namespace SamplePanels
