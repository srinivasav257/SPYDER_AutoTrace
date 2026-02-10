#include "ActivityRail.h"
#include "IconManager.h"
#include "ThemeManager.h"

#include <QAction>
#include <QActionGroup>
#include <QEvent>
#include <QWidget>

namespace DockManager {

ActivityRail::ActivityRail(QWidget* parent)
    : QToolBar(parent)
{
    setObjectName("ActivityRail");
    setOrientation(Qt::Vertical);
    setMovable(false);
    setFloatable(false);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(QSize(24, 24));
    setContextMenuPolicy(Qt::PreventContextMenu);
    setAllowedAreas(Qt::LeftToolBarArea);
    setMinimumWidth(40);
    setMaximumWidth(40);

    StyleLib::ThemeManager::instance().applyScopedStyle(this, StyleLib::ScopedStyle::ActivityRail);

    auto* taskGroup = new QActionGroup(this);
    taskGroup->setExclusive(true);

    m_testDashboardAction = addAction(QIcon(), tr("Test Dashboard"));
    m_testDashboardAction->setCheckable(true);
    m_testDashboardAction->setActionGroup(taskGroup);
    connect(m_testDashboardAction, &QAction::triggered, this, [this]() {
        emit taskRequested(QStringLiteral("test_dashboard"));
    });

    m_canalyzerAction = addAction(QIcon(), tr("CANalyzer"));
    m_canalyzerAction->setCheckable(true);
    m_canalyzerAction->setActionGroup(taskGroup);
    connect(m_canalyzerAction, &QAction::triggered, this, [this]() {
        emit taskRequested(QStringLiteral("canalyzer"));
    });

    auto* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    addWidget(spacer);

    m_settingsAction = addAction(QIcon(), tr("Settings"));
    connect(m_settingsAction, &QAction::triggered, this, [this]() {
        emit utilityRequested(QStringLiteral("settings"));
    });

    m_profileAction = addAction(QIcon(), tr("Profile"));
    connect(m_profileAction, &QAction::triggered, this, [this]() {
        emit utilityRequested(QStringLiteral("profile"));
    });

    refreshIcons();
}

void ActivityRail::setActiveTask(const QString& taskId)
{
    if (taskId == QLatin1String("test_dashboard")) {
        m_testDashboardAction->setChecked(true);
    } else if (taskId == QLatin1String("canalyzer")) {
        m_canalyzerAction->setChecked(true);
    } else {
        m_testDashboardAction->setChecked(false);
        m_canalyzerAction->setChecked(false);
    }
}

void ActivityRail::refreshIcons()
{
    if (m_testDashboardAction) {
        m_testDashboardAction->setIcon(Icons::icon(Icons::Id::ActivityDashboard, this));
    }
    if (m_canalyzerAction) {
        m_canalyzerAction->setIcon(Icons::icon(Icons::Id::ActivityCanalyzer, this));
    }
    if (m_settingsAction) {
        m_settingsAction->setIcon(Icons::icon(Icons::Id::ActivitySettings, this));
    }
    if (m_profileAction) {
        m_profileAction->setIcon(Icons::icon(Icons::Id::ActivityProfile, this));
    }
}

void ActivityRail::changeEvent(QEvent* event)
{
    QToolBar::changeEvent(event);

    if (event->type() == QEvent::PaletteChange ||
        event->type() == QEvent::StyleChange) {
        refreshIcons();
    }
}

} // namespace DockManager
