#include "ThemeManager.h"

#include "PerformanceProxyStyle.h"

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QStyle>
#include <QWidget>

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
    app.setStyleSheet(styleSheetFor(ScopedStyle::Application));
    applyTheme(app, themeId);
}

void ThemeManager::applyTheme(QApplication& app, ThemeId themeId)
{
    m_currentTheme = themeId;
    app.setPalette(buildPalette(themeId));
    app.setProperty("spyder.theme", themeKey(themeId));
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

    widget->setStyleSheet(styleSheetFor(scope));
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
    }

    return QString();
}

} // namespace StyleLib
