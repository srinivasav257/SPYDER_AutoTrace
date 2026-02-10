#include "FramelessTopBar.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QToolButton>
#include <QWindow>

namespace DockManager {

namespace {

enum class TitleButtonGlyph
{
    Minimize,
    Maximize,
    Restore,
    Close
};

QColor titleButtonIconColor()
{
    return QColor(QStringLiteral("#E0E0E1"));
}

QIcon makeTitleButtonIcon(TitleButtonGlyph glyph, const QColor& color)
{
    constexpr int kSize = 14;
    QPixmap pixmap(kSize, kSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(color, 1.0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(Qt::NoBrush);

    switch (glyph) {
    case TitleButtonGlyph::Minimize:
        painter.drawLine(QPointF(2.5, 10.5), QPointF(11.5, 10.5));
        break;
    case TitleButtonGlyph::Maximize:
        painter.drawRect(QRectF(2.5, 2.5, 9.0, 9.0));
        break;
    case TitleButtonGlyph::Restore:
        painter.drawRect(QRectF(4.5, 2.5, 7.0, 6.5));
        painter.drawRect(QRectF(2.5, 4.5, 7.0, 6.5));
        break;
    case TitleButtonGlyph::Close:
        painter.drawLine(QPointF(3.0, 3.0), QPointF(11.0, 11.0));
        painter.drawLine(QPointF(11.0, 3.0), QPointF(3.0, 11.0));
        break;
    }

    return QIcon(pixmap);
}

} // namespace

FramelessTopBar::FramelessTopBar(QWidget* hostWindow, QWidget* parent)
    : QWidget(parent)
    , m_hostWindow(hostWindow)
{
    setObjectName("framelessTopBar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(38);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 3, 4, 1);
    layout->setSpacing(4);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->setScaledContents(true);

    m_menuBar = new QMenuBar(this);
    m_menuBar->setNativeMenuBar(false);
    m_menuBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    m_minimizeButton = new QToolButton(this);
    m_minimizeButton->setObjectName("titleMinimizeButton");
    m_minimizeButton->setAutoRaise(true);
    m_minimizeButton->setFixedSize(40, 28);
    m_minimizeButton->setIconSize(QSize(14, 14));
    m_minimizeButton->setToolTip(tr("Minimize"));
    m_minimizeButton->setIcon(makeTitleButtonIcon(TitleButtonGlyph::Minimize, titleButtonIconColor()));

    m_maximizeButton = new QToolButton(this);
    m_maximizeButton->setObjectName("titleMaximizeButton");
    m_maximizeButton->setAutoRaise(true);
    m_maximizeButton->setFixedSize(40, 28);
    m_maximizeButton->setIconSize(QSize(14, 14));

    m_closeButton = new QToolButton(this);
    m_closeButton->setObjectName("titleCloseButton");
    m_closeButton->setAutoRaise(true);
    m_closeButton->setFixedSize(40, 28);
    m_closeButton->setIconSize(QSize(14, 14));
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setIcon(makeTitleButtonIcon(TitleButtonGlyph::Close, titleButtonIconColor()));

    layout->addWidget(m_iconLabel);
    layout->addSpacing(10);
    layout->addWidget(m_menuBar);
    layout->addStretch(1);
    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(m_closeButton);

    connect(m_minimizeButton, &QToolButton::clicked, this, [this]() {
        if (m_hostWindow) {
            m_hostWindow->showMinimized();
        }
    });
    connect(m_maximizeButton, &QToolButton::clicked, this, [this]() {
        toggleMaximizeRestore();
    });
    connect(m_closeButton, &QToolButton::clicked, this, [this]() {
        if (m_hostWindow) {
            m_hostWindow->close();
        }
    });

    if (m_hostWindow) {
        m_hostWindow->installEventFilter(this);
    }

    syncFromHostWindow();
}

QMenuBar* FramelessTopBar::menuBar() const
{
    return m_menuBar;
}

bool FramelessTopBar::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_hostWindow) {
        switch (event->type()) {
        case QEvent::WindowTitleChange:
        case QEvent::WindowIconChange:
            syncFromHostWindow();
            break;
        case QEvent::WindowStateChange:
            updateWindowStateButton();
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void FramelessTopBar::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isDragRegion(event->position().toPoint())) {
        if (m_hostWindow && m_hostWindow->windowHandle()) {
            m_hostWindow->windowHandle()->startSystemMove();
            event->accept();
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void FramelessTopBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isDragRegion(event->position().toPoint())) {
        toggleMaximizeRestore();
        event->accept();
        return;
    }

    QWidget::mouseDoubleClickEvent(event);
}

void FramelessTopBar::syncFromHostWindow()
{
    if (!m_hostWindow) {
        return;
    }

    const QPixmap iconPixmap = m_hostWindow->windowIcon().pixmap(16, 16);
    if (iconPixmap.isNull()) {
        m_iconLabel->setPixmap(style()->standardIcon(QStyle::SP_DesktopIcon).pixmap(16, 16));
    } else {
        m_iconLabel->setPixmap(iconPixmap);
    }

    updateWindowStateButton();
}

void FramelessTopBar::updateWindowStateButton()
{
    if (!m_hostWindow || !m_maximizeButton) {
        return;
    }

    const bool isMaximized = m_hostWindow->isMaximized();
    if (isMaximized) {
        m_maximizeButton->setToolTip(tr("Restore"));
        m_maximizeButton->setIcon(makeTitleButtonIcon(TitleButtonGlyph::Restore, titleButtonIconColor()));
    } else {
        m_maximizeButton->setToolTip(tr("Maximize"));
        m_maximizeButton->setIcon(makeTitleButtonIcon(TitleButtonGlyph::Maximize, titleButtonIconColor()));
    }
}

void FramelessTopBar::toggleMaximizeRestore()
{
    if (!m_hostWindow) {
        return;
    }

    if (m_hostWindow->isMaximized()) {
        m_hostWindow->showNormal();
    } else {
        m_hostWindow->showMaximized();
    }
}

bool FramelessTopBar::isDragRegion(const QPoint& localPos) const
{
    QWidget* child = childAt(localPos);
    if (!child) {
        return true;
    }

    if (child == m_minimizeButton || child == m_maximizeButton || child == m_closeButton) {
        return false;
    }

    if (m_menuBar && (child == m_menuBar || m_menuBar->isAncestorOf(child))) {
        return false;
    }

    return true;
}

} // namespace DockManager
