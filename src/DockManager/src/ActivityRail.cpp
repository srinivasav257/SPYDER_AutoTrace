#include "ActivityRail.h"
#include "ThemeManager.h"

#include <QAction>
#include <QActionGroup>
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
    setIconSize(QSize(20, 20));
    setContextMenuPolicy(Qt::PreventContextMenu);
    setAllowedAreas(Qt::LeftToolBarArea);
    setMinimumWidth(46);

    StyleLib::ThemeManager::instance().applyScopedStyle(this, StyleLib::ScopedStyle::ActivityRail);

    auto* taskGroup = new QActionGroup(this);
    taskGroup->setExclusive(true);

    m_testDashboardAction = addAction(QIcon(":/icons/WindowStyle/tests_explorer.png"), tr("Test Dashboard"));
    m_testDashboardAction->setCheckable(true);
    m_testDashboardAction->setActionGroup(taskGroup);
    connect(m_testDashboardAction, &QAction::triggered, this, [this]() {
        emit taskRequested(QStringLiteral("test_dashboard"));
    });

    m_canalyzerAction = addAction(QIcon(":/icons/WindowStyle/connected_3D.png"), tr("CANalyzer"));
    m_canalyzerAction->setCheckable(true);
    m_canalyzerAction->setActionGroup(taskGroup);
    connect(m_canalyzerAction, &QAction::triggered, this, [this]() {
        emit taskRequested(QStringLiteral("canalyzer"));
    });

    addSeparator();

    auto* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    addWidget(spacer);

    addSeparator();

    QAction* settingsAction = addAction(QIcon(":/icons/WindowStyle/settings.png"), tr("Settings"));
    connect(settingsAction, &QAction::triggered, this, [this]() {
        emit utilityRequested(QStringLiteral("settings"));
    });

    QAction* profileAction = addAction(QIcon(":/icons/WindowStyle/profile.png"), tr("Profile"));
    connect(profileAction, &QAction::triggered, this, [this]() {
        emit utilityRequested(QStringLiteral("profile"));
    });
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

} // namespace DockManager
