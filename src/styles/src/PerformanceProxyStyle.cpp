#include "PerformanceProxyStyle.h"

namespace StyleLib {

PerformanceProxyStyle::PerformanceProxyStyle()
    : QProxyStyle(QStringLiteral("Fusion"))
{
}

int PerformanceProxyStyle::pixelMetric(PixelMetric metric,
                                       const QStyleOption* option,
                                       const QWidget* widget) const
{
    switch (metric) {
    case PM_DefaultFrameWidth:
        return 1;
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
        return 8;
    case PM_ButtonMargin:
        return 6;
    case PM_ScrollBarExtent:
        return 14;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

} // namespace StyleLib
