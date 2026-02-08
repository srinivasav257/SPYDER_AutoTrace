#pragma once

#include "ThemeCatalog.h"
#include <QPalette>

class QApplication;
class QWidget;

namespace StyleLib {

enum class ScopedStyle : int
{
    Application = 0,
    ActivityRail,
    WelcomePage
};

class ThemeManager
{
public:
    static ThemeManager& instance();

    void initialize(QApplication& app, ThemeId themeId);
    void applyTheme(QApplication& app, ThemeId themeId);

    ThemeId currentTheme() const;
    QString currentThemeKey() const;

    void applyScopedStyle(QWidget* widget, ScopedStyle scope) const;

private:
    ThemeManager() = default;

    void ensurePerformanceStyle(QApplication& app) const;
    QPalette buildPalette(ThemeId id) const;
    QString styleSheetFor(ScopedStyle scope) const;

    ThemeId m_currentTheme = ThemeId::LightLavender;
};

} // namespace StyleLib
