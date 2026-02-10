#include "ThemeManager.h"

#include "PerformanceProxyStyle.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QStyle>
#include <QWidget>

#ifdef Q_OS_WIN
#include <QWindow>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

namespace StyleLib {
namespace {

QString loadStyleSheetFromResource(const QString& resourcePath)
{
    QFile styleFile(resourcePath);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    return QString::fromUtf8(styleFile.readAll());
}

void setGroupPaletteColors(QPalette& palette,
                           QPalette::ColorGroup group,
                           const ThemeDefinition& theme)
{
    palette.setColor(group, QPalette::Window, theme.window);
    palette.setColor(group, QPalette::WindowText, theme.text);
    palette.setColor(group, QPalette::Base, theme.base);
    palette.setColor(group, QPalette::AlternateBase, theme.alternateBase);
    palette.setColor(group, QPalette::ToolTipBase, theme.tooltipBase);
    palette.setColor(group, QPalette::ToolTipText, theme.tooltipText);
    palette.setColor(group, QPalette::Text, theme.text);
    palette.setColor(group, QPalette::Button, theme.button);
    palette.setColor(group, QPalette::ButtonText, theme.buttonText);
    palette.setColor(group, QPalette::BrightText, theme.highlightedText);
    palette.setColor(group, QPalette::Highlight, theme.highlight);
    palette.setColor(group, QPalette::HighlightedText, theme.highlightedText);
    palette.setColor(group, QPalette::Mid, theme.mid);
    palette.setColor(group, QPalette::Midlight, theme.mid.lighter(115));
    palette.setColor(group, QPalette::Dark, theme.mid.darker(140));
    palette.setColor(group, QPalette::Shadow, theme.mid.darker(180));
    palette.setColor(group, QPalette::PlaceholderText, theme.mid);
    palette.setColor(group, QPalette::Link, theme.highlight);
    palette.setColor(group, QPalette::LinkVisited, theme.highlight.darker(130));
}

} // namespace

ThemeManager& ThemeManager::instance()
{
    static ThemeManager manager;
    return manager;
}

void ThemeManager::initialize(QApplication& app, ThemeId themeId)
{
    ensurePerformanceStyle(app);
    applyTheme(app, themeId);
}

void ThemeManager::applyTheme(QApplication& app, ThemeId themeId)
{
    m_currentTheme = themeId;
    app.setPalette(buildPalette(themeId));
    app.setProperty("spyder.theme", themeKey(themeId));

    // Force stylesheet reparse so palette(...) roles refresh immediately on theme switch.
    const QString appStyle = styleSheetFor(ScopedStyle::Application);
    app.setStyleSheet(QString());
    app.setStyleSheet(appStyle);

    // Reapply scoped styles that were registered through applyScopedStyle().
    const int minScope = static_cast<int>(ScopedStyle::Application);
    const int maxScope = static_cast<int>(ScopedStyle::HWConfig);
    for (QWidget* widget : app.allWidgets()) {
        if (!widget) {
            continue;
        }

        const auto scopeValue = widget->property("spyder.styleScope");
        if (!scopeValue.isValid()) {
            continue;
        }

        bool ok = false;
        const int scopeId = scopeValue.toInt(&ok);
        if (!ok || scopeId < minScope || scopeId > maxScope) {
            continue;
        }

        widget->setStyleSheet(styleSheetFor(static_cast<ScopedStyle>(scopeId)));
        widget->update();
    }
}

ThemeId ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

QString ThemeManager::currentThemeKey() const
{
    return themeKey(m_currentTheme);
}

void ThemeManager::applyScopedStyle(QWidget* widget, ScopedStyle scope) const
{
    if (!widget) {
        return;
    }

    widget->setProperty("spyder.styleScope", static_cast<int>(scope));
    widget->setStyleSheet(styleSheetFor(scope));
}

void ThemeManager::setDarkTitleBar(QWidget* window)
{
#ifdef Q_OS_WIN
    if (!window) {
        return;
    }

    window->winId(); // ensure native window handle is created
    QWindow* qWin = window->windowHandle();
    if (!qWin) {
        return;
    }

    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(reinterpret_cast<HWND>(qWin->winId()),
                          DWMWA_USE_IMMERSIVE_DARK_MODE,
                          &darkMode, sizeof(darkMode));
#else
    Q_UNUSED(window);
#endif
}

void ThemeManager::ensurePerformanceStyle(QApplication& app) const
{
    if (dynamic_cast<PerformanceProxyStyle*>(app.style()) != nullptr) {
        return;
    }

    app.setStyle(new PerformanceProxyStyle());
}

QPalette ThemeManager::buildPalette(ThemeId id) const
{
    const ThemeDefinition& theme = themeDefinition(id);

    QPalette palette;
    setGroupPaletteColors(palette, QPalette::Active, theme);
    setGroupPaletteColors(palette, QPalette::Inactive, theme);
    setGroupPaletteColors(palette, QPalette::Disabled, theme);

    const QColor disabledText = theme.text.darker(145);
    const QColor disabledMid = theme.mid.darker(125);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);
    palette.setColor(QPalette::Disabled, QPalette::Mid, disabledMid);
    palette.setColor(QPalette::Disabled, QPalette::PlaceholderText, disabledMid);
    palette.setColor(QPalette::Disabled, QPalette::Highlight, theme.highlight.darker(130));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledText);

    return palette;
}

QString ThemeManager::styleSheetFor(ScopedStyle scope) const
{
    switch (scope) {
    case ScopedStyle::Application: {
        static const QString appStyle = loadStyleSheetFromResource(QStringLiteral(":/styles/application.qss"));
        return appStyle;
    }
    case ScopedStyle::ActivityRail: {
        static const QString activityRailStyle = loadStyleSheetFromResource(QStringLiteral(":/styles/activity_rail.qss"));
        return activityRailStyle;
    }
    case ScopedStyle::WelcomePage: {
        static const QString welcomePageStyle = loadStyleSheetFromResource(QStringLiteral(":/styles/welcome_page.qss"));
        return welcomePageStyle;
    }
    case ScopedStyle::TestExecutor: {
        static const QString testExecutorStyle = loadStyleSheetFromResource(QStringLiteral(":/styles/test_executor.qss"));
        return testExecutorStyle;
    }
    case ScopedStyle::HWConfig: {
        static const QString hwConfigStyle = loadStyleSheetFromResource(QStringLiteral(":/styles/hw_config.qss"));
        return hwConfigStyle;
    }
    }

    return QString();
}

} // namespace StyleLib
