#include <DockFramework.h>
#include "HWConfigManager.h"
#include "HWConfigDialog.h"
#include "SamplePanels.h"
#include "TestExecutorPanels.h"
#include "TestRepository.h"
#include "ThemeCatalog.h"
#include "ThemeManager.h"
#include <ManDiag.h>
#include <DBCManager.h>
#include <QApplication>
#include <QMenuBar>
#include <QSplashScreen>
#include <QPainter>
#include <QElapsedTimer>
#include <QSettings>
#include <QFileInfo>
#include <QIcon>

// Helper: create a programmatic splash image (no external resource needed)
static QPixmap createSplashPixmap()
{
    QPixmap pix(480, 280);
    pix.fill(QColor(30, 30, 30));

    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    // Accent bar
    p.fillRect(0, 0, 480, 6, QColor(0, 120, 215));

    // App name
    QFont titleFont("Segoe UI", 28, QFont::Bold);
    p.setFont(titleFont);
    p.setPen(Qt::white);
    p.drawText(pix.rect().adjusted(0, 50, 0, -80), Qt::AlignCenter, "SPYDER AutoTrace");

    // Version
    QFont verFont("Segoe UI", 12);
    p.setFont(verFont);
    p.setPen(QColor(180, 180, 180));
    p.drawText(pix.rect().adjusted(0, 110, 0, -40), Qt::AlignCenter,
               QString("v%1").arg(QApplication::applicationVersion().isEmpty()
                                      ? "1.0.0"
                                      : QApplication::applicationVersion()));

    // Loading text
    QFont loadFont("Segoe UI", 10);
    p.setFont(loadFont);
    p.setPen(QColor(140, 140, 140));
    p.drawText(pix.rect().adjusted(20, 0, -20, -18), Qt::AlignBottom | Qt::AlignHCenter,
               "Initializing...");

    p.end();
    return pix;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icons/app_icon_square.svg"));

    // Set application metadata (used by QSettings for state persistence)
    QApplication::setApplicationName("AutoTrace");
    QApplication::setOrganizationName("SPYDER");
    QApplication::setApplicationVersion("1.0.0");

    {
        QSettings settings;
        const QString savedThemeKey = settings.value(
            "UI/theme",
            StyleLib::themeKey(StyleLib::ThemeId::LightLavender)).toString();
        const StyleLib::ThemeId startupTheme = StyleLib::themeIdFromKey(savedThemeKey);
        StyleLib::ThemeManager::instance().initialize(app, startupTheme);
    }

    // --- Splash screen ---
    QSplashScreen splash(createSplashPixmap());
    splash.show();
    app.processEvents();

    auto showStatus = [&](const QString& msg) {
        splash.showMessage(msg, Qt::AlignBottom | Qt::AlignHCenter, QColor(180, 180, 180));
        app.processEvents();
    };

    showStatus("Loading hardware configuration...");
    // Initialize HW configuration (loads saved settings)
    HWConfigManager::instance().applyToSerialManager();

    showStatus("Loading DBC databases...");
    // Auto-load saved DBC files for CAN channels (background parsing)
    DBCManager::DBCDatabaseManager::instance().loadSavedPaths();

    showStatus("Registering test panels...");
    // Register all panel types before creating the window
    TestExecutor::registerTestExecutorPanels();
    SamplePanels::registerSamplePanels();

    showStatus("Loading test repository...");
    auto& testRepo = TestExecutor::TestRepository::instance();

    QObject::connect(&testRepo, &TestExecutor::TestRepository::repositorySaved,
                     &app, [](const QString& filePath) {
        QSettings settings;
        settings.setValue("TestExecutor/lastRepositoryFile", filePath);
    });
    QObject::connect(&testRepo, &TestExecutor::TestRepository::repositoryLoaded,
                     &app, [](const QString& filePath) {
        QSettings settings;
        settings.setValue("TestExecutor/lastRepositoryFile", filePath);
    });

    {
        QSettings settings;
        const QString lastRepositoryFile = settings.value("TestExecutor/lastRepositoryFile").toString();
        if (!lastRepositoryFile.isEmpty() && QFileInfo::exists(lastRepositoryFile)) {
            testRepo.loadFromFile(lastRepositoryFile);
        }
    }

    showStatus("Registering ManDiag commands...");
    // Register ManDiag commands (EOL and MOL)
    ManDiag::registerAllCommands();

    showStatus("Building main window...");
    // Create and show the dock main window
    DockManager::DockMainWindow window;
    window.setWindowTitle("SPYDER AutoTrace");
    window.setWindowIcon(app.windowIcon());

    // Add Tools menu with HW Configuration dialog
    // Insert before the last menu (Help) - find the Perspectives menu position
    auto* toolsMenu = new QMenu(QObject::tr("&Tools"), &window);
    toolsMenu->addAction(QObject::tr("HW Configuration..."), &window, [&window]() {
        HWConfigDialog dlg(&window);
        dlg.exec();
    });
    // Insert Tools menu before the Help menu (last menu)
    auto* menuBar = window.appMenuBar();
    auto actions = menuBar->actions();
    if (!actions.isEmpty()) {
        menuBar->insertMenu(actions.last(), toolsMenu);
    } else {
        menuBar->addMenu(toolsMenu);
    }

    window.show();

    // Close splash once main window is visible
    splash.finish(&window);

    return app.exec();
}
