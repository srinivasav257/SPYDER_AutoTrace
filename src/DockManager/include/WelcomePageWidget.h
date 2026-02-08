#pragma once

#include <QWidget>

class QLabel;
class QIcon;

namespace DockManager {

/**
 * @brief Centered welcome content shown when no dock panel is open.
 */
class WelcomePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomePageWidget(QWidget* parent = nullptr);

    void setAppIcon(const QIcon& icon);

signals:
    void shortcutRequested(const QString& shortcutId);

private:
    QLabel* m_iconLabel = nullptr;
};

} // namespace DockManager
