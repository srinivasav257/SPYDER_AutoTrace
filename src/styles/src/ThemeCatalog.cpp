#include "ThemeCatalog.h"

namespace StyleLib {
namespace {

const QVector<ThemeDefinition> kThemes{
    {
        ThemeId::DarkGreenBlue,
        QStringLiteral("hackers_curiosity"),
        QStringLiteral("Hacker's Curiosity"),
        QColor(QStringLiteral("#322628")),
        QColor(QStringLiteral("#000000")),
        QColor(QStringLiteral("#655B55")),
        QColor(QStringLiteral("#E0E0E1")),
        QColor(QStringLiteral("#322628")),
        QColor(QStringLiteral("#E0E0E1")),
        QColor(QStringLiteral("#655B55")),
        QColor(QStringLiteral("#D26E41")),
        QColor(QStringLiteral("#0A181E")),
        QColor(QStringLiteral("#322628")),
        QColor(QStringLiteral("#A6A5A2"))
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
