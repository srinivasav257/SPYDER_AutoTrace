#pragma once

#include <QWidget>

class QLabel;
class QMenuBar;
class QToolButton;
class QEvent;
class QMouseEvent;
class QPoint;

namespace DockManager {

/**
 * @brief Window top bar used for frameless windows.
 *
 * Hosts:
 * - Application icon
 * - Standard menu bar
 * - Minimize/Maximize/Close buttons
 *
 * The widget tracks title/icon/state changes from the host window.
 */
class FramelessTopBar : public QWidget
{
public:
    explicit FramelessTopBar(QWidget* hostWindow, QWidget* parent = nullptr);

    QMenuBar* menuBar() const;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void syncFromHostWindow();
    void updateWindowStateButton();
    void toggleMaximizeRestore();
    bool isDragRegion(const QPoint& localPos) const;

    QWidget* m_hostWindow = nullptr;
    QLabel* m_iconLabel = nullptr;
    QMenuBar* m_menuBar = nullptr;
    QToolButton* m_minimizeButton = nullptr;
    QToolButton* m_maximizeButton = nullptr;
    QToolButton* m_closeButton = nullptr;
};

} // namespace DockManager
