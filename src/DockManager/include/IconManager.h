#pragma once

#include <QIcon>
class QWidget;

namespace DockManager::Icons {

enum class Id
{
    Import,
    Save,
    AddGroup,
    AddFeature,
    AddTest,
    Remove,
    ExpandAll,
    CollapseAll,
    PanelExplorer,
    PanelProgress,
    GroupItem,
    FeatureItem,
    TestItem,
    Run
};

QIcon icon(Id id, const QWidget* context = nullptr);

} // namespace DockManager::Icons
