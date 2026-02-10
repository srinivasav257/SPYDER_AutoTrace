#include "ThemeCatalog.h"

namespace StyleLib {
namespace {

const QVector<ThemeDefinition> kThemes{
    {
        ThemeId::DarkGreenBlue,
        QStringLiteral("dark_acqua_green_blue"),
        QStringLiteral("Dark Acqua Green Blue"),
        QColor(QStringLiteral("#132028")),
        QColor(QStringLiteral("#0F1A21")),
        QColor(QStringLiteral("#182833")),
        QColor(QStringLiteral("#E6EDF3")),
        QColor(QStringLiteral("#1B2B36")),
        QColor(QStringLiteral("#E6EDF3")),
        QColor(QStringLiteral("#304250")),
        QColor(QStringLiteral("#3FB950")),
        QColor(QStringLiteral("#0F1A21")),
        QColor(QStringLiteral("#1B2B36")),
        QColor(QStringLiteral("#E6EDF3"))
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
