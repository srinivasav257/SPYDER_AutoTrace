/**
 * @file TestExecutorPanels.h
 * @brief Registration of TestExecutor panels with the DockManager.
 * 
 * This header provides a function to register all TestExecutor panels
 * with the DockManager::PanelRegistry. Call this before creating the
 * DockMainWindow.
 */

#ifndef TESTEXECUTOR_PANELS_H
#define TESTEXECUTOR_PANELS_H

#include "PanelRegistry.h"

namespace TestExecutor {

/**
 * @brief Register all TestExecutor panels with the DockManager
 * 
 * This function registers the following panels:
 * - test_explorer: Test Explorer panel for browsing and managing tests
 * - test_editor: Test Editor panel for editing test cases and steps
 * - test_progress: Test Progress panel for monitoring execution
 * 
 * @return true if all panels registered successfully
 */
bool registerTestExecutorPanels();

} // namespace TestExecutor

#endif // TESTEXECUTOR_PANELS_H
