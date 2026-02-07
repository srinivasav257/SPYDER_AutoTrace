/**
 * @file TestExecutorPanels.cpp
 * @brief Implementation of TestExecutor panel registration.
 */

#include "TestExecutorPanels.h"
#include "TestExplorerPanel.h"
#include "TestProgressPanel.h"
#include "IconManager.h"
#include <DockWidget.h>

namespace TestExecutor {

bool registerTestExecutorPanels()
{
    auto& registry = DockManager::PanelRegistry::instance();
    bool success = true;
    
    // Test Explorer Panel - for browsing and managing test cases
    success &= registry.registerPanel({
        .id = "test_explorer",
        .title = "Test Explorer",
        .category = "Test Automation",
        .defaultArea = ads::LeftDockWidgetArea,
        .factory = [](QWidget* parent) -> QWidget* {
            return new TestExplorerPanel(parent);
        },
        .icon = DockManager::Icons::icon(DockManager::Icons::Id::PanelExplorer)
    });
    
    // Test Progress Panel - for monitoring test execution
    success &= registry.registerPanel({
        .id = "test_progress",
        .title = "Test Progress",
        .category = "Test Automation",
        .defaultArea = ads::BottomDockWidgetArea,
        .factory = [](QWidget* parent) -> QWidget* {
            return new TestProgressPanel(parent);
        },
        .icon = DockManager::Icons::icon(DockManager::Icons::Id::PanelProgress)
    });
    
    return success;
}

} // namespace TestExecutor
