#include "DockMainWindow.h"
#include "PanelRegistry.h"
#include "WorkspaceManager.h"
#include "DockToolBar.h"
#include "IconManager.h"
#include "FramelessTopBar.h"
#include "WelcomePageWidget.h"
#include "ActivityRail.h"

#include "DockManager.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QCloseEvent>
#include <QEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QInputDialog>
#include <QSet>
#include <QSignalBlocker>
#include <QStringList>
#include <QTimer>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windowsx.h>
#endif

namespace DockManager {

struct DockMainWindow::Private
{
    ads::CDockManager* dockManager = nullptr;
    WorkspaceManager* workspaceManager = nullptr;
    FramelessTopBar* topBar = nullptr;
    WelcomePageWidget* welcomePage = nullptr;
    ActivityRail* activityRail = nullptr;
    DockToolBar* dockToolBar = nullptr;

    QMap<QString, ads::CDockWidget*> dockWidgets;
    QMap<QString, QAction*> panelToggleActions;
    ads::CDockAreaWidget* centralArea = nullptr;
    QMenu* perspectiveMenu = nullptr;
};

DockMainWindow::DockMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , d(new Private)
{
    // Default window setup
    resize(1400, 900);
    statusBar()->showMessage(tr("Ready"));

    // Initialize in order
    configureFlags();

    // Create dock manager
    d->dockManager = new ads::CDockManager(this);
    d->dockManager->setStyleSheet(QString());

    // Create workspace manager
    d->workspaceManager = new WorkspaceManager(d->dockManager, this);

    // Create panels and layout
    createPanels();
    createTopBar();
    createMenus();
    createActivityRail();
    setupDefaultLayout();
    createWelcomePage();

    // Load saved perspectives FIRST (before saving Default)
    d->workspaceManager->loadPerspectives();

    // Save default layout as "Default" perspective (only if not already present)
    if (!d->workspaceManager->perspectiveNames().contains("Default")) {
        d->workspaceManager->savePerspective("Default");
    }

    // Try to restore previous session
    auto savedGeometry = d->workspaceManager->savedGeometry();
    if (!savedGeometry.isEmpty()) {
        restoreGeometry(savedGeometry);
    }

    const bool restoredState = d->workspaceManager->restoreState();
    if (!restoredState) {
        activateTaskGroup(QStringLiteral("test_dashboard"));
    }
    updateWelcomePageVisibility();
    QTimer::singleShot(0, this, [this]() {
        updateWelcomePageVisibility();
    });

    // Update perspective menu
    rebuildPerspectiveMenu();

    // Signal initialization complete
    initializeComplete();
}

DockMainWindow::~DockMainWindow() = default;

// --- Accessors ---

ads::CDockManager* DockMainWindow::dockManager() const
{
    return d->dockManager;
}

WorkspaceManager* DockMainWindow::workspaceManager() const
{
    return d->workspaceManager;
}

DockToolBar* DockMainWindow::dockToolBar() const
{
    return d->dockToolBar;
}

ActivityRail* DockMainWindow::activityRail() const
{
    return d->activityRail;
}

ads::CDockWidget* DockMainWindow::dockWidget(const QString& panelId) const
{
    return d->dockWidgets.value(panelId);
}

QMap<QString, ads::CDockWidget*> DockMainWindow::dockWidgets() const
{
    return d->dockWidgets;
}

QMenuBar* DockMainWindow::appMenuBar() const
{
    if (d->topBar && d->topBar->menuBar()) {
        return d->topBar->menuBar();
    }

    return QMainWindow::menuBar();
}

ads::CDockAreaWidget* DockMainWindow::centralArea() const
{
    return d->centralArea;
}

QMenu* DockMainWindow::perspectiveMenu() const
{
    return d->perspectiveMenu;
}

// --- Virtual Implementation Methods ---

void DockMainWindow::configureFlags()
{
    // Configure flags BEFORE creating the dock manager
    ads::CDockManager::setConfigFlags(ads::CDockManager::DefaultOpaqueConfig);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);

    // Enable auto-hide (sidebar pinning)
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);
}

void DockMainWindow::createPanels()
{
    const auto& panels = PanelRegistry::instance().panels();

    for (const auto& def : panels) {
        // Use def.id as the object name for state save/restore
        auto* dockWidget = new ads::CDockWidget(def.id, d->dockManager);
        dockWidget->setWindowTitle(def.title);

        // Set icon if provided
        if (!def.icon.isNull()) {
            dockWidget->setIcon(def.icon);
        }

        // Create content widget using factory
        QWidget* content = def.factory(dockWidget);
        dockWidget->setWidget(content, ads::CDockWidget::ForceNoScrollArea);

        // Configure features
        dockWidget->setFeatures(def.features);
        dockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, false);
        dockWidget->setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromContent);

        d->dockWidgets.insert(def.id, dockWidget);

        connect(dockWidget, &ads::CDockWidget::viewToggled, this, [this](bool) {
            updateWelcomePageVisibility();
        });
        if (QAction* toggleAction = dockWidget->toggleViewAction()) {
            connect(toggleAction, &QAction::toggled, this, [this](bool) {
                updateWelcomePageVisibility();
            });
        }
    }
}

void DockMainWindow::setupDefaultLayout()
{
    ads::CDockAreaWidget* leftArea = nullptr;
    ads::CDockAreaWidget* rightArea = nullptr;
    ads::CDockAreaWidget* bottomArea = nullptr;
    ads::CDockAreaWidget* topArea = nullptr;
    ads::CDockAreaWidget* centerArea = nullptr;

    const auto& panels = PanelRegistry::instance().panels();

    // Pass 1: Place the first panel per area to establish the dock areas
    for (const auto& def : panels) {
        auto* dw = d->dockWidgets.value(def.id);
        if (!dw) continue;

        switch (def.defaultArea) {
        case ads::LeftDockWidgetArea:
            if (!leftArea)
                leftArea = d->dockManager->addDockWidget(ads::LeftDockWidgetArea, dw);
            break;
        case ads::RightDockWidgetArea:
            if (!rightArea)
                rightArea = d->dockManager->addDockWidget(ads::RightDockWidgetArea, dw);
            break;
        case ads::BottomDockWidgetArea:
            if (!bottomArea)
                bottomArea = d->dockManager->addDockWidget(ads::BottomDockWidgetArea, dw);
            break;
        case ads::TopDockWidgetArea:
            if (!topArea)
                topArea = d->dockManager->addDockWidget(ads::TopDockWidgetArea, dw);
            break;
        case ads::CenterDockWidgetArea:
        default:
            if (!centerArea)
                centerArea = d->dockManager->addDockWidget(ads::CenterDockWidgetArea, dw);
            break;
        }
    }

    d->centralArea = centerArea;

    // Pass 2: Tab remaining panels into their area
    for (const auto& def : panels) {
        auto* dw = d->dockWidgets.value(def.id);
        if (!dw || dw->dockAreaWidget()) continue; // already placed

        ads::CDockAreaWidget* targetArea = nullptr;
        switch (def.defaultArea) {
        case ads::LeftDockWidgetArea:   targetArea = leftArea;   break;
        case ads::RightDockWidgetArea:  targetArea = rightArea;  break;
        case ads::BottomDockWidgetArea: targetArea = bottomArea; break;
        case ads::TopDockWidgetArea:    targetArea = topArea;    break;
        default:                        targetArea = centerArea; break;
        }

        if (targetArea)
            d->dockManager->addDockWidgetTabToArea(dw, targetArea);
        else
            d->dockManager->addDockWidget(def.defaultArea, dw);
    }
}

void DockMainWindow::createTopBar()
{
    setWindowFlag(Qt::FramelessWindowHint, true);

    d->topBar = new FramelessTopBar(this, this);
    setMenuWidget(d->topBar);
}

void DockMainWindow::createWelcomePage()
{
    if (!d->dockManager || d->welcomePage) {
        return;
    }

    d->welcomePage = new WelcomePageWidget(d->dockManager);
    d->welcomePage->setAppIcon(windowIcon().isNull() ? QApplication::windowIcon() : windowIcon());
    d->welcomePage->hide();

    connect(d->welcomePage, &WelcomePageWidget::shortcutRequested, this, [this](const QString& shortcutId) {
        if (shortcutId == QLatin1String("open_test_explorer")) {
            if (auto* dw = d->dockWidgets.value("test_explorer")) {
                dw->toggleView(true);
            }
            return;
        }

        if (shortcutId == QLatin1String("open_test_progress")) {
            if (auto* dw = d->dockWidgets.value("test_progress")) {
                dw->toggleView(true);
            }
            return;
        }

        if (shortcutId == QLatin1String("quick_start_dummy")) {
            statusBar()->showMessage(tr("Quick Start will be implemented in next steps."), 2500);
        }
    });

    d->dockManager->installEventFilter(this);
    syncWelcomePageGeometry();
    updateWelcomePageVisibility();
}

void DockMainWindow::createActivityRail()
{
    if (d->activityRail) {
        return;
    }

    d->activityRail = new ActivityRail(this);
    addToolBar(Qt::LeftToolBarArea, d->activityRail);

    connect(d->activityRail, &ActivityRail::taskRequested, this, [this](const QString& taskId) {
        activateTaskGroup(taskId);
    });

    connect(d->activityRail, &ActivityRail::utilityRequested, this, [this](const QString& utilityId) {
        if (utilityId == QLatin1String("settings")) {
            statusBar()->showMessage(tr("Settings panel will be added later."), 2500);
            return;
        }

        if (utilityId == QLatin1String("profile")) {
            statusBar()->showMessage(tr("Profile panel will be added later."), 2500);
        }
    });
}

void DockMainWindow::createMenus()
{
    auto* appBar = appMenuBar();
    if (!appBar) {
        return;
    }

    // --- File menu ---
    auto* fileMenu = appBar->addMenu(tr("&File"));
    fileMenu->addAction(tr("Save Layout"), this, [this]() {
        d->workspaceManager->saveState();
        statusBar()->showMessage(tr("Layout saved"), 3000);
    });
    fileMenu->addAction(tr("Restore Layout"), this, [this]() {
        d->workspaceManager->restoreState();
        statusBar()->showMessage(tr("Layout restored"), 3000);
    });
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit, this, &QMainWindow::close);

    // --- View menu (auto-populated by category) ---
    auto* viewMenu = appBar->addMenu(tr("&View"));

    // Add toggle actions grouped by category
    for (const auto& category : PanelRegistry::instance().categories()) {
        auto* catMenu = viewMenu->addMenu(category);
        for (const auto& def : PanelRegistry::instance().panelsInCategory(category)) {
            auto* dw = d->dockWidgets.value(def.id);
            if (dw)
                catMenu->addAction(dw->toggleViewAction());
        }
    }

    viewMenu->addSeparator();
    viewMenu->addAction(tr("Show All Panels"), this, [this]() {
        for (auto* dw : d->dockWidgets)
            dw->toggleView(true);
        if (d->activityRail) {
            d->activityRail->setActiveTask(QString());
        }
    });
    viewMenu->addAction(tr("Hide All Panels"), this, [this]() {
        for (auto* dw : d->dockWidgets)
            dw->toggleView(false);
        if (d->activityRail) {
            d->activityRail->setActiveTask(QString());
        }
    });

    // --- Perspectives menu ---
    d->perspectiveMenu = appBar->addMenu(tr("&Perspectives"));

    // Connect to workspace manager for menu updates
    connect(d->workspaceManager, &WorkspaceManager::perspectiveSaved,
            this, &DockMainWindow::rebuildPerspectiveMenu);

    // --- Help menu ---
    auto* helpMenu = appBar->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About"), this, [this]() {
        QMessageBox::about(this, tr("About"),
            tr("<h3>%1</h3>"
               "<p>Qt Advanced Docking System Template</p>"
               "<p>Panels registered: %2<br>"
               "Categories: %3</p>")
            .arg(QApplication::applicationName())
            .arg(PanelRegistry::instance().count())
            .arg(PanelRegistry::instance().categories().join(", ")));
    });
}

void DockMainWindow::createToolBar()
{
    // Legacy top toolbar intentionally disabled.
    d->dockToolBar = nullptr;
}

void DockMainWindow::initializeComplete()
{
    // Default implementation does nothing
    // Override in derived class for custom initialization
}

void DockMainWindow::refreshIcons()
{
    auto updateActionEnabledState = [this](const QString& panelId) {
        auto* dockWidget = d->dockWidgets.value(panelId);
        auto* action = d->panelToggleActions.value(panelId);
        if (!dockWidget || !action) {
            return;
        }
        const bool visible = dockWidget->isVisible();
        QSignalBlocker block(action);
        action->setChecked(visible);
        action->setEnabled(!visible);
    };

    auto setPanelIcon = [this](const QString& panelId, const QIcon& icon) {
        auto* dockWidget = d->dockWidgets.value(panelId);
        if (!dockWidget) {
            return;
        }

        dockWidget->setIcon(icon);

        if (QAction* toggleAction = dockWidget->toggleViewAction()) {
            toggleAction->setIcon(icon);
        }
    };

    const QIcon explorerIcon = DockManager::Icons::icon(DockManager::Icons::Id::PanelExplorer, this);
    const QIcon progressIcon = DockManager::Icons::icon(DockManager::Icons::Id::PanelProgress, this);

    setPanelIcon("test_explorer", explorerIcon);
    setPanelIcon("test_progress", progressIcon);

    if (d->activityRail) {
        d->activityRail->refreshIcons();
    }

    updateActionEnabledState("test_explorer");
    updateActionEnabledState("test_progress");
}

void DockMainWindow::syncWelcomePageGeometry()
{
    if (!d->dockManager || !d->welcomePage) {
        return;
    }

    d->welcomePage->setGeometry(d->dockManager->rect());
}

bool DockMainWindow::hasOpenDockWidgets() const
{
    for (auto* dockWidget : d->dockWidgets) {
        if (!dockWidget) {
            continue;
        }

        QAction* toggleAction = dockWidget->toggleViewAction();
        if (toggleAction && toggleAction->isChecked()) {
            return true;
        }
    }

    return false;
}

void DockMainWindow::updateWelcomePageVisibility()
{
    if (!d->welcomePage) {
        return;
    }

    auto isPanelVisible = [this](const QString& panelId) {
        auto* dockWidget = d->dockWidgets.value(panelId);
        if (!dockWidget) {
            return false;
        }
        QAction* toggleAction = dockWidget->toggleViewAction();
        return toggleAction && toggleAction->isChecked();
    };

    int visibleCount = 0;
    for (auto* dockWidget : d->dockWidgets) {
        if (!dockWidget) {
            continue;
        }
        QAction* toggleAction = dockWidget->toggleViewAction();
        if (toggleAction && toggleAction->isChecked()) {
            ++visibleCount;
        }
    }

    if (d->activityRail) {
        const bool isTestDashboard = visibleCount == 2 &&
                                     isPanelVisible(QStringLiteral("test_explorer")) &&
                                     isPanelVisible(QStringLiteral("test_progress"));
        const bool isCanalyzer = visibleCount == 2 &&
                                 isPanelVisible(QStringLiteral("can_trace")) &&
                                 isPanelVisible(QStringLiteral("ig_block"));

        if (isTestDashboard) {
            d->activityRail->setActiveTask(QStringLiteral("test_dashboard"));
        } else if (isCanalyzer) {
            d->activityRail->setActiveTask(QStringLiteral("canalyzer"));
        } else {
            d->activityRail->setActiveTask(QString());
        }
    }

    const bool showWelcome = !hasOpenDockWidgets();
    if (showWelcome) {
        syncWelcomePageGeometry();
        d->welcomePage->show();
        d->welcomePage->raise();
    } else {
        d->welcomePage->hide();
    }
}

void DockMainWindow::showOnlyPanels(const QStringList& panelIds)
{
    const QSet<QString> targetPanels(panelIds.begin(), panelIds.end());

    for (auto it = d->dockWidgets.constBegin(); it != d->dockWidgets.constEnd(); ++it) {
        auto* dw = it.value();
        if (!dw) {
            continue;
        }

        const bool shouldBeVisible = targetPanels.contains(it.key());
        dw->toggleView(shouldBeVisible);
    }

    // Bring requested panels to foreground in requested order.
    for (const QString& panelId : panelIds) {
        if (auto* dw = d->dockWidgets.value(panelId)) {
            dw->raise();
        }
    }

    updateWelcomePageVisibility();
}

void DockMainWindow::activateTaskGroup(const QString& taskId)
{
    if (taskId == QLatin1String("test_dashboard")) {
        showOnlyPanels({QStringLiteral("test_explorer"), QStringLiteral("test_progress")});
        if (d->activityRail) {
            d->activityRail->setActiveTask(taskId);
        }
        statusBar()->showMessage(tr("Switched to Test Dashboard"), 2000);
        return;
    }

    if (taskId == QLatin1String("canalyzer")) {
        showOnlyPanels({QStringLiteral("can_trace"), QStringLiteral("ig_block")});
        if (d->activityRail) {
            d->activityRail->setActiveTask(taskId);
        }
        statusBar()->showMessage(tr("Switched to CANalyzer"), 2000);
    }
}

void DockMainWindow::rebuildPerspectiveMenu()
{
    if (!d->perspectiveMenu)
        return;

    d->perspectiveMenu->clear();

    // Save perspective action
    d->perspectiveMenu->addAction(tr("Save Perspective..."), this, [this]() {
        bool ok = false;
        const QString name = QInputDialog::getText(
            this,
            tr("Save Perspective"),
            tr("Perspective name:"),
            QLineEdit::Normal,
            QString(),
            &ok).trimmed();

        if (ok && !name.isEmpty()) {
            d->workspaceManager->savePerspective(name);
            statusBar()->showMessage(tr("Perspective '%1' saved").arg(name), 3000);
        }
    });

    d->perspectiveMenu->addSeparator();

    // List all perspectives
    for (const auto& name : d->workspaceManager->perspectiveNames()) {
        d->perspectiveMenu->addAction(name, this, [this, name]() {
            d->workspaceManager->loadPerspective(name);
            statusBar()->showMessage(tr("Perspective '%1' loaded").arg(name), 3000);
        });
    }
}

void DockMainWindow::closeEvent(QCloseEvent* event)
{
    // Save state before closing
    d->workspaceManager->saveGeometry(saveGeometry());
    d->workspaceManager->saveState();
    d->workspaceManager->savePerspectives();

    QMainWindow::closeEvent(event);
}

void DockMainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);

    if (event->type() == QEvent::PaletteChange ||
        event->type() == QEvent::StyleChange) {
        refreshIcons();
        updateWelcomePageVisibility();
    }
}

bool DockMainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d->dockManager && d->welcomePage) {
        if (event->type() == QEvent::Resize ||
            event->type() == QEvent::Show ||
            event->type() == QEvent::LayoutRequest) {
            syncWelcomePageGeometry();
            if (d->welcomePage->isVisible()) {
                d->welcomePage->raise();
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

#ifdef Q_OS_WIN
bool DockMainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
    Q_UNUSED(eventType)

    if (isMaximized() || isFullScreen()) {
        return QMainWindow::nativeEvent(eventType, message, result);
    }

    MSG* msg = static_cast<MSG*>(message);
    if (msg && msg->message == WM_NCHITTEST) {
        constexpr LONG kResizeBorder = 8;

        RECT windowRect{};
        if (!GetWindowRect(reinterpret_cast<HWND>(winId()), &windowRect)) {
            return QMainWindow::nativeEvent(eventType, message, result);
        }

        const bool canResizeWidth = minimumWidth() < maximumWidth();
        const bool canResizeHeight = minimumHeight() < maximumHeight();

        const LONG globalX = GET_X_LPARAM(msg->lParam);
        const LONG globalY = GET_Y_LPARAM(msg->lParam);

        const bool onLeft = canResizeWidth &&
                            globalX >= windowRect.left &&
                            globalX < windowRect.left + kResizeBorder;
        const bool onRight = canResizeWidth &&
                             globalX < windowRect.right &&
                             globalX >= windowRect.right - kResizeBorder;
        const bool onTop = canResizeHeight &&
                           globalY >= windowRect.top &&
                           globalY < windowRect.top + kResizeBorder;
        const bool onBottom = canResizeHeight &&
                              globalY < windowRect.bottom &&
                              globalY >= windowRect.bottom - kResizeBorder;

        if (onTop && onLeft) {
            *result = HTTOPLEFT;
            return true;
        }
        if (onTop && onRight) {
            *result = HTTOPRIGHT;
            return true;
        }
        if (onBottom && onLeft) {
            *result = HTBOTTOMLEFT;
            return true;
        }
        if (onBottom && onRight) {
            *result = HTBOTTOMRIGHT;
            return true;
        }
        if (onLeft) {
            *result = HTLEFT;
            return true;
        }
        if (onRight) {
            *result = HTRIGHT;
            return true;
        }
        if (onTop) {
            *result = HTTOP;
            return true;
        }
        if (onBottom) {
            *result = HTBOTTOM;
            return true;
        }
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}
#endif

} // namespace DockManager
