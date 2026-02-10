#include "ThemeCatalog.h"

namespace StyleLib {
namespace {

const QVector<ThemeDefinition> kThemes{
    {
        ThemeId::DarkGreenBlue,
        QStringLiteral("hackers_curiosity"),
        QStringLiteral("Hacker's Curiosity"),
        QColor(QStringLiteral("#0A0D0F")),          // window
        QColor(QStringLiteral("#000000")),          // base
        QColor(QStringLiteral("#1D252A")),          // alternateBase
        QColor(QStringLiteral("#E0E0E1")),          // text
        QColor(QStringLiteral("#1D252A")),          // button
        QColor(QStringLiteral("#E0E0E1")),          // buttonText
        QColor(QStringLiteral("#4d4b4b")),          // mid
        QColor(QStringLiteral("#D26E41")),          // highlight
        QColor(QStringLiteral("#0A181E")),          // highlightedText
        QColor(QStringLiteral("#1D252A")),          // tooltipBase
        QColor(QStringLiteral("#A6A5A2"))           // tooltipText
    }
};

} // namespace

const QVector<ThemeDefinition>& availableThemes()
{
    return kThemes;
}

const ThemeDefinition& themeDefinition(ThemeId id)
{
    for (const ThemeDefinition& theme : kThemes) {
        if (theme.id == id) {
            return theme;
        }
    }

    return kThemes.front();
}

ThemeId themeIdFromKey(const QString& key)
{
    if (key.compare(QStringLiteral("dark_green_blue"), Qt::CaseInsensitive) == 0) {
        return ThemeId::DarkGreenBlue;
    }
    if (key.compare(QStringLiteral("dark_acqua_green_blue"), Qt::CaseInsensitive) == 0) {
        return ThemeId::DarkGreenBlue;
    }

    for (const ThemeDefinition& theme : kThemes) {
        if (theme.key.compare(key, Qt::CaseInsensitive) == 0) {
            return theme.id;
        }
    }

    return ThemeId::DarkGreenBlue;
}

QString themeKey(ThemeId id)
{
    return themeDefinition(id).key;
}

} // namespace StyleLib
