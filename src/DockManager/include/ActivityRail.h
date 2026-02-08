#pragma once

#include <QToolBar>

class QAction;
class QEvent;

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
    void refreshIcons();

signals:
    void taskRequested(const QString& taskId);
    void utilityRequested(const QString& utilityId);

protected:
    void changeEvent(QEvent* event) override;

private:
    QAction* m_testDashboardAction = nullptr;
    QAction* m_canalyzerAction = nullptr;
    QAction* m_settingsAction = nullptr;
    QAction* m_profileAction = nullptr;
};

} // namespace DockManager
