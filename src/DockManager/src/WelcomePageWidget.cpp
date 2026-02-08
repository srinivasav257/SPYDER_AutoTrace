#include "WelcomePageWidget.h"
#include "ThemeManager.h"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace DockManager {

WelcomePageWidget::WelcomePageWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName("welcomePage");
    setAttribute(Qt::WA_StyledBackground, true);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 32, 32, 32);
    rootLayout->addStretch(1);

    auto* card = new QFrame(this);
    card->setObjectName("welcomeCard");
    card->setMaximumWidth(560);

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(28, 24, 28, 24);
    cardLayout->setSpacing(14);

    m_iconLabel = new QLabel(card);
    m_iconLabel->setFixedSize(88, 88);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setScaledContents(true);

    auto* title = new QLabel(tr("Welcome to %1").arg(QApplication::applicationName()), card);
    title->setObjectName("welcomeTitle");
    title->setAlignment(Qt::AlignCenter);

    auto* subtitle = new QLabel(tr("All panels are currently closed. Use a shortcut to get started."), card);
    subtitle->setObjectName("welcomeSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);

    auto* shortcutsContainer = new QWidget(card);
    auto* shortcutsLayout = new QHBoxLayout(shortcutsContainer);
    shortcutsLayout->setContentsMargins(0, 0, 0, 0);
    shortcutsLayout->setSpacing(10);

    auto* explorerBtn = new QPushButton(tr("Open Test Explorer"), shortcutsContainer);
    explorerBtn->setObjectName("welcomeShortcutButton");
    auto* progressBtn = new QPushButton(tr("Open Test Progress"), shortcutsContainer);
    progressBtn->setObjectName("welcomeShortcutButton");
    auto* quickStartBtn = new QPushButton(tr("Quick Start (Dummy)"), shortcutsContainer);
    quickStartBtn->setObjectName("welcomeShortcutButton");

    shortcutsLayout->addWidget(explorerBtn);
    shortcutsLayout->addWidget(progressBtn);
    shortcutsLayout->addWidget(quickStartBtn);

    cardLayout->addWidget(m_iconLabel, 0, Qt::AlignHCenter);
    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addWidget(shortcutsContainer);

    rootLayout->addWidget(card, 0, Qt::AlignHCenter);
    rootLayout->addStretch(2);

    connect(explorerBtn, &QPushButton::clicked, this, [this]() {
        emit shortcutRequested(QStringLiteral("open_test_explorer"));
    });
    connect(progressBtn, &QPushButton::clicked, this, [this]() {
        emit shortcutRequested(QStringLiteral("open_test_progress"));
    });
    connect(quickStartBtn, &QPushButton::clicked, this, [this]() {
        emit shortcutRequested(QStringLiteral("quick_start_dummy"));
    });

    StyleLib::ThemeManager::instance().applyScopedStyle(this, StyleLib::ScopedStyle::WelcomePage);
}

void WelcomePageWidget::setAppIcon(const QIcon& icon)
{
    const QPixmap pix = icon.pixmap(88, 88);
    if (!pix.isNull()) {
        m_iconLabel->setPixmap(pix);
    } else {
        m_iconLabel->setPixmap(QIcon::fromTheme(QStringLiteral("applications-system")).pixmap(88, 88));
    }
}

} // namespace DockManager
