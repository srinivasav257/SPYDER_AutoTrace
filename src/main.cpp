#include <DockFramework.h>
#include "SamplePanels.h"
#include "HWConfigManager.h"
#include "HWConfigDialog.h"
#include "TestExecutorPanels.h"
#include <ManDiag.h>
#include <DBCManager.h>
#include <QApplication>
#include <QMenuBar>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata (used by QSettings for state persistence)
    QApplication::setApplicationName("AutoTrace");
    QApplication::setOrganizationName("SPYDER");
    QApplication::setApplicationVersion("1.0.0");

    // Initialize HW configuration (loads saved settings)
    HWConfigManager::instance().applyToSerialManager();

    // Auto-load saved DBC files for CAN channels (background parsing)
    DBCManager::DBCDatabaseManager::instance().loadSavedPaths();

    // Register all panel types before creating the window
    registerSamplePanels();
    TestExecutor::registerTestExecutorPanels();

    // Register ManDiag commands (EOL and MOL)
    ManDiag::registerAllCommands();

    // Create and show the dock main window
    DockManager::DockMainWindow window;
    window.setWindowTitle("SPYDER AutoTrace");

    // Add Tools menu with HW Configuration dialog
    // Insert before the last menu (Help) - find the Perspectives menu position
    auto* toolsMenu = new QMenu(QObject::tr("&Tools"), &window);
    toolsMenu->addAction(QObject::tr("HW Configuration..."), &window, [&window]() {
        HWConfigDialog dlg(&window);
        dlg.exec();
    });
    // Insert Tools menu before the Help menu (last menu)
    auto* menuBar = window.menuBar();
    auto actions = menuBar->actions();
    if (!actions.isEmpty()) {
        menuBar->insertMenu(actions.last(), toolsMenu);
    } else {
        menuBar->addMenu(toolsMenu);
    }

    window.show();

    return app.exec();
}
