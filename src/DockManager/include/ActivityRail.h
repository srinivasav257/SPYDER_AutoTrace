#pragma once

#include <QToolBar>

class QAction;

namespace DockManager {

/**
 * @brief VS Code style left activity rail for high-level task switching.
 */
class ActivityRail : public QToolBar
{
    Q_OBJECT

public:
    explicit ActivityRail(QWidget* parent = nullptr);

    void setActiveTask(const QString& taskId);

signals:
    void taskRequested(const QString& taskId);
    void utilityRequested(const QString& utilityId);

private:
    QAction* m_testDashboardAction = nullptr;
    QAction* m_canalyzerAction = nullptr;
};

} // namespace DockManager
