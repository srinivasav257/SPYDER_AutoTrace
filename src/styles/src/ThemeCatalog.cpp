#include "ThemeCatalog.h"

namespace StyleLib {
namespace {

const QVector<ThemeDefinition> kThemes{
    {
        ThemeId::LightLavender,
        QStringLiteral("light_lavender"),
        QStringLiteral("Light Lavender"),
        QColor(QStringLiteral("#FFFFFF")),
        QColor(QStringLiteral("#F7F6FC")),
        QColor(QStringLiteral("#EEEAFB")),
        QColor(QStringLiteral("#1F2430")),
        QColor(QStringLiteral("#ECE8FA")),
        QColor(QStringLiteral("#1F2430")),
        QColor(QStringLiteral("#C8C2E0")),
        QColor(QStringLiteral("#7C4DFF")),
        QColor(QStringLiteral("#FFFFFF")),
        QColor(QStringLiteral("#FFFFFF")),
        QColor(QStringLiteral("#1F2430"))
    },
    {
        ThemeId::DarkGreenBlue,
        QStringLiteral("dark_green_blue"),
        QStringLiteral("Dark Green Blue"),
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
    },
    {
        ThemeId::DarkOrangePurple,
        QStringLiteral("dark_orange_purple"),
        QStringLiteral("Dark Orange Purple"),
        QColor(QStringLiteral("#211826")),
        QColor(QStringLiteral("#1A1320")),
        QColor(QStringLiteral("#2A1F33")),
        QColor(QStringLiteral("#F4EEF9")),
        QColor(QStringLiteral("#2D2238")),
        QColor(QStringLiteral("#F4EEF9")),
        QColor(QStringLiteral("#433651")),
        QColor(QStringLiteral("#FF6B35")),
        QColor(QStringLiteral("#1A1320")),
        QColor(QStringLiteral("#2D2238")),
        QColor(QStringLiteral("#F4EEF9"))
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
    for (const ThemeDefinition& theme : kThemes) {
        if (theme.key.compare(key, Qt::CaseInsensitive) == 0) {
            return theme.id;
        }
    }

    return ThemeId::LightLavender;
}

QString themeKey(ThemeId id)
{
    return themeDefinition(id).key;
}

} // namespace StyleLib
