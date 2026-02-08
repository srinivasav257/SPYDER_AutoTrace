#pragma once

#include <QProxyStyle>

namespace StyleLib {

class PerformanceProxyStyle : public QProxyStyle
{
public:
    PerformanceProxyStyle();

    int pixelMetric(PixelMetric metric,
                    const QStyleOption* option = nullptr,
                    const QWidget* widget = nullptr) const override;
};

} // namespace StyleLib
