#include <DockFramework.h>
#include "SamplePanels.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application metadata (used by QSettings for state persistence)
    QApplication::setApplicationName("AutoTrace");
    QApplication::setOrganizationName("SPYDER");
    QApplication::setApplicationVersion("1.0.0");

    // Register all panel types before creating the window
    registerSamplePanels();

    // Create and show the dock main window
    DockManager::DockMainWindow window;
    window.setWindowTitle("SPYDER AutoTrace");
    window.show();

    return app.exec();
}
