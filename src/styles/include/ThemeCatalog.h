#pragma once

#include <QColor>
#include <QString>
#include <QVector>

namespace StyleLib {

enum class ThemeId : int
{
    LightLavender = 0,
    DarkGreenBlue,
    DarkOrangePurple
};

struct ThemeDefinition
{
    ThemeId id;
    QString key;
    QString displayName;

    QColor window;
    QColor base;
    QColor alternateBase;
    QColor text;
    QColor button;
    QColor buttonText;
    QColor mid;
    QColor highlight;
    QColor highlightedText;
    QColor tooltipBase;
    QColor tooltipText;
};

const QVector<ThemeDefinition>& availableThemes();
const ThemeDefinition& themeDefinition(ThemeId id);
ThemeId themeIdFromKey(const QString& key);
QString themeKey(ThemeId id);

} // namespace StyleLib
