#include "IconManager.h"

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QWidget>
#include <functional>
#include <QtMath>

namespace DockManager::Icons
{

    namespace
    {

        constexpr int kIconSize = 18;
        constexpr int kActivityRailIconSize = 26;

        bool isActivityRailContext(const QWidget *context)
        {
            return context && context->objectName() == QStringLiteral("ActivityRail");
        }

        int iconCanvasSize(const QWidget *context)
        {
            return isActivityRailContext(context) ? kActivityRailIconSize : kIconSize;
        }

        QStyle *activeStyle(const QWidget *context)
        {
            if (context)
            {
                return context->style();
            }
            if (qApp)
            {
                return qApp->style();
            }

            return nullptr;
        }

        QColor iconColor(const QWidget *context)
        {
            if (isActivityRailContext(context))
            {
                return QColor(QStringLiteral("#A6A5A2"));
            }
            if (context)
            {
                return context->palette().color(QPalette::WindowText);
            }
            if (qApp)
            {
                return qApp->palette().color(QPalette::WindowText);
            }

            return QColor(QStringLiteral("#8F97B2"));
        }

        QColor disabledIconColor(const QColor &color)
        {
            QColor disabled = color;
            disabled.setAlpha(110);
            return disabled;
        }

        QColor activeIconColor(const QWidget *context, const QColor &fallback)
        {
            if (isActivityRailContext(context))
            {
                return QColor(QStringLiteral("#FFFFFF"));
            }
            if (context)
            {
                return context->palette().color(QPalette::HighlightedText);
            }
            if (qApp)
            {
                return qApp->palette().color(QPalette::HighlightedText);
            }

            return fallback;
        }

        QPen linePen(const QColor &color, qreal width = 1.6)
        {
            QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            return pen;
        }

        void drawPlus(QPainter &painter, const QPointF &center, qreal half, const QColor &color)
        {
            painter.setPen(linePen(color, 1.5));
            painter.drawLine(QPointF(center.x() - half, center.y()),
                             QPointF(center.x() + half, center.y()));
            painter.drawLine(QPointF(center.x(), center.y() - half),
                             QPointF(center.x(), center.y() + half));
        }

        void drawFolder(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            const qreal left = rect.left() + 1.0;
            const qreal top = rect.top() + 3.0;
            const qreal right = rect.right() - 1.0;
            const qreal bottom = rect.bottom() - 1.5;

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            QPainterPath path;
            path.moveTo(left, top + 4.0);
            path.lineTo(left + 4.0, top + 4.0);
            path.lineTo(left + 6.0, top + 2.2);
            path.lineTo(right, top + 2.2);
            path.lineTo(right, bottom);
            path.lineTo(left, bottom);
            path.closeSubpath();
            painter.drawPath(path);
        }

        void drawDocument(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            QRectF body(rect.left() + 3.0, rect.top() + 2.0, rect.width() - 6.0, rect.height() - 4.0);

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(body, 1.4, 1.4);

            const qreal foldX = body.right() - 4.0;
            const qreal foldY = body.top() + 4.0;
            painter.drawLine(QPointF(foldX, body.top()), QPointF(body.right(), foldY));
            painter.drawLine(QPointF(foldX, body.top()), QPointF(foldX, foldY));
            painter.drawLine(QPointF(foldX, foldY), QPointF(body.right(), foldY));
        }

        void drawDiamond(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            const QPointF center = rect.center();
            const qreal dx = 4.8;
            const qreal dy = 4.8;

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);
            painter.drawPolygon(QPolygonF{
                QPointF(center.x(), center.y() - dy),
                QPointF(center.x() + dx, center.y()),
                QPointF(center.x(), center.y() + dy),
                QPointF(center.x() - dx, center.y())});
        }

        void drawTrayArrow(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            const qreal left = rect.left() + 3.0;
            const qreal right = rect.right() - 3.0;
            const qreal trayY = rect.bottom() - 3.2;
            const qreal cx = rect.center().x();
            const qreal top = rect.top() + 2.6;
            const qreal shaftBottom = trayY - 2.2;

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            painter.drawLine(QPointF(left, trayY), QPointF(right, trayY));
            painter.drawLine(QPointF(left, trayY), QPointF(left + 1.3, trayY - 2.5));
            painter.drawLine(QPointF(right, trayY), QPointF(right - 1.3, trayY - 2.5));

            painter.drawLine(QPointF(cx, top), QPointF(cx, shaftBottom));
            painter.drawLine(QPointF(cx, shaftBottom), QPointF(cx - 2.5, shaftBottom - 2.5));
            painter.drawLine(QPointF(cx, shaftBottom), QPointF(cx + 2.5, shaftBottom - 2.5));
        }

        void drawSaveDisk(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            QRectF body(rect.left() + 2.2, rect.top() + 2.2, rect.width() - 4.4, rect.height() - 4.4);

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(body, 1.8, 1.8);

            painter.drawLine(QPointF(body.left() + 2.2, body.top() + 4.2),
                             QPointF(body.right() - 2.2, body.top() + 4.2));
            painter.drawRect(QRectF(body.left() + 3.0, body.center().y(),
                                    body.width() - 6.0, body.height() * 0.28));
        }

        void drawRemove(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            const QPointF c = rect.center();
            const qreal radius = 5.2;

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(c, radius, radius);
            painter.drawLine(QPointF(c.x() - 2.8, c.y()), QPointF(c.x() + 2.8, c.y()));
        }

        void drawChevronPair(QPainter &painter, const QRectF &rect, const QColor &color, bool down)
        {
            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            const qreal left = rect.left() + 4.2;
            const qreal right = rect.right() - 4.2;
            const qreal mid = rect.center().x();

            const qreal y1 = down ? rect.top() + 6.2 : rect.bottom() - 6.2;
            const qreal y2 = down ? y1 + 3.4 : y1 - 3.4;
            const qreal y3 = down ? y2 + 3.6 : y2 - 3.6;
            const qreal y4 = down ? y3 + 3.4 : y3 - 3.4;

            painter.drawLine(QPointF(left, y1), QPointF(mid, y2));
            painter.drawLine(QPointF(mid, y2), QPointF(right, y1));
            painter.drawLine(QPointF(left, y3), QPointF(mid, y4));
            painter.drawLine(QPointF(mid, y4), QPointF(right, y3));
        }

        void drawExplorer(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            const qreal x0 = rect.left() + 2.8;
            const qreal x1 = rect.left() + 5.8;
            const qreal x2 = rect.right() - 2.4;

            const qreal y1 = rect.top() + 4.2;
            const qreal y2 = rect.center().y();
            const qreal y3 = rect.bottom() - 3.2;

            for (qreal y : {y1, y2, y3})
            {
                painter.drawEllipse(QPointF(x0, y), 0.8, 0.8);
                painter.drawLine(QPointF(x1, y), QPointF(x2, y));
            }
        }

        void drawProgress(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            const qreal left = rect.left() + 3.0;
            const qreal bottom = rect.bottom() - 2.5;
            const qreal right = rect.right() - 2.5;
            const qreal top = rect.top() + 3.0;

            painter.drawLine(QPointF(left, top), QPointF(left, bottom));
            painter.drawLine(QPointF(left, bottom), QPointF(right, bottom));

            painter.drawRect(QRectF(left + 2.0, bottom - 4.0, 2.4, 4.0));
            painter.drawRect(QRectF(left + 6.0, bottom - 7.0, 2.4, 7.0));
            painter.drawRect(QRectF(left + 10.0, bottom - 10.0, 2.4, 10.0));
        }

        void drawPlay(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            painter.setPen(linePen(color, 1.3));
            painter.setBrush(color);

            const qreal left = rect.left() + 5.0;
            const qreal top = rect.top() + 3.2;
            const qreal bottom = rect.bottom() - 3.2;
            const qreal right = rect.right() - 3.8;

            painter.drawPolygon(QPolygonF{
                QPointF(left, top),
                QPointF(right, rect.center().y()),
                QPointF(left, bottom)});
        }

        void drawDashboard(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            // Render a bolder 2x2 block grid so the dashboard icon stays crisp on larger activity-rail sizes.
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);

            const qreal cell = rect.width() * 0.28;
            const qreal gap = rect.width() * 0.12;
            const qreal cluster = (2.0 * cell) + gap;
            const qreal startX = rect.center().x() - (cluster / 2.0);
            const qreal startY = rect.center().y() - (cluster / 2.0);
            const qreal radius = qMax<qreal>(1.0, cell * 0.22);

            painter.drawRoundedRect(QRectF(startX, startY, cell, cell), radius, radius);
            painter.drawRoundedRect(QRectF(startX + cell + gap, startY, cell, cell), radius, radius);
            painter.drawRoundedRect(QRectF(startX, startY + cell + gap, cell, cell), radius, radius);
            painter.drawRoundedRect(QRectF(startX + cell + gap, startY + cell + gap, cell, cell), radius, radius);
        }

        void drawCanalyzer(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            const qreal left = rect.left() + 2.0;
            const qreal right = rect.right() - 2.0;
            const qreal top = rect.top() + 4.0;
            const qreal bottom = rect.bottom() - 4.0;

            QPainterPath wave;
            wave.moveTo(left, bottom - 1.0);
            wave.lineTo(left + 2.8, bottom - 1.0);
            wave.lineTo(left + 4.8, top + 1.6);
            wave.lineTo(left + 7.2, bottom - 3.4);
            wave.lineTo(left + 10.0, top + 3.0);
            wave.lineTo(right, top + 3.0);
            painter.drawPath(wave);
        }

        void drawSettings(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            const QPointF c = rect.center();
            const qreal inner = rect.width() * 0.19;
            const qreal outer = rect.width() * 0.34;

            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            for (int i = 0; i < 8; ++i)
            {
                const qreal a = qDegreesToRadians(45.0 * static_cast<qreal>(i));
                const QPointF p1(c.x() + qCos(a) * inner, c.y() + qSin(a) * inner);
                const QPointF p2(c.x() + qCos(a) * outer, c.y() + qSin(a) * outer);
                painter.drawLine(p1, p2);
            }

            painter.drawEllipse(c, rect.width() * 0.17, rect.width() * 0.17);
        }

        void drawProfile(QPainter &painter, const QRectF &rect, const QColor &color)
        {
            const QPointF c = rect.center();
            painter.setPen(linePen(color));
            painter.setBrush(Qt::NoBrush);

            const qreal headRadius = rect.width() * 0.17;
            const qreal shoulderHalfWidth = rect.width() * 0.30;
            const qreal shoulderTop = rect.bottom() - rect.height() * 0.40;
            const qreal shoulderBottom = rect.bottom() - rect.height() * 0.12;
            painter.drawEllipse(QPointF(c.x(), rect.top() + rect.height() * 0.30), headRadius, headRadius);

            QPainterPath shoulders;
            shoulders.moveTo(c.x() - shoulderHalfWidth, shoulderBottom);
            shoulders.cubicTo(c.x() - shoulderHalfWidth, shoulderTop,
                              c.x() + shoulderHalfWidth, shoulderTop,
                              c.x() + shoulderHalfWidth, shoulderBottom);
            painter.drawPath(shoulders);
        }

        QIcon buildIcon(const QWidget *context,
                        const std::function<void(QPainter &, const QRectF &, const QColor &)> &draw)
        {
            const int iconSize = iconCanvasSize(context);
            const auto render = [&](const QColor &color)
            {
                QPixmap pixmap(iconSize, iconSize);
                pixmap.fill(Qt::transparent);

                QPainter painter(&pixmap);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::TextAntialiasing, true);
                draw(painter, QRectF(1.0, 1.0, iconSize - 2.0, iconSize - 2.0), color);
                return pixmap;
            };

            const QColor normal = iconColor(context);
            QColor hover = normal;
            if (isActivityRailContext(context))
            {
                hover = QColor(QStringLiteral("#E0E0E1"));
            }
            const QColor active = activeIconColor(context, normal);
            const QColor disabled = disabledIconColor(normal);

            QIcon icon;
            icon.addPixmap(render(normal), QIcon::Normal, QIcon::Off);
            icon.addPixmap(render(hover), QIcon::Active, QIcon::Off);
            icon.addPixmap(render(hover), QIcon::Selected, QIcon::Off);
            icon.addPixmap(render(active), QIcon::Normal, QIcon::On);
            icon.addPixmap(render(active), QIcon::Active, QIcon::On);
            icon.addPixmap(render(active), QIcon::Selected, QIcon::On);
            icon.addPixmap(render(disabled), QIcon::Disabled, QIcon::Off);
            icon.addPixmap(render(disabled), QIcon::Disabled, QIcon::On);
            return icon;
        }

    } // namespace

    QIcon icon(Id id, const QWidget *context)
    {
        Q_UNUSED(activeStyle(context));

        switch (id)
        {
        case Id::Import:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawTrayArrow(p, r, c); });
        case Id::Save:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawSaveDisk(p, r, c); });
        case Id::AddGroup:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             {
            drawFolder(p, r, c);
            drawPlus(p, QPointF(r.right() - 3.0, r.bottom() - 3.2), 2.0, c); });
        case Id::AddFeature:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             {
            drawDiamond(p, r, c);
            drawPlus(p, QPointF(r.right() - 3.0, r.bottom() - 3.2), 2.0, c); });
        case Id::AddTest:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             {
            drawDocument(p, r, c);
            drawPlus(p, QPointF(r.right() - 3.0, r.bottom() - 3.2), 2.0, c); });
        case Id::Remove:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawRemove(p, r, c); });
        case Id::ExpandAll:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawChevronPair(p, r, c, true); });
        case Id::CollapseAll:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawChevronPair(p, r, c, false); });
        case Id::PanelExplorer:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawExplorer(p, r, c); });
        case Id::PanelProgress:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawProgress(p, r, c); });
        case Id::GroupItem:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawFolder(p, r, c); });
        case Id::FeatureItem:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawDiamond(p, r, c); });
        case Id::TestItem:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawDocument(p, r, c); });
        case Id::Run:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawPlay(p, r, c); });
        case Id::ActivityDashboard:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawDashboard(p, r, c); });
        case Id::ActivityCanalyzer:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawCanalyzer(p, r, c); });
        case Id::ActivitySettings:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawSettings(p, r, c); });
        case Id::ActivityProfile:
            return buildIcon(context, [](QPainter &p, const QRectF &r, const QColor &c)
                             { drawProfile(p, r, c); });
        }

        return QIcon();
    }

} // namespace DockManager::Icons
