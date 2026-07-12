#include "canvas.h"

#include <algorithm>
#include <QPainter>
#include <QResizeEvent>

namespace {
QPointF mapPoint(const QPointF& point, const QRectF& src, const QRectF& dst)
{
    if (src.width() <= 0.0 || src.height() <= 0.0) {
        return dst.center();
    }

    const qreal x = dst.left() + (point.x() - src.left()) * dst.width() / src.width();
    const qreal y = dst.top() + (point.y() - src.top()) * dst.height() / src.height();
    return {x, y};
}
}

Canvas::Canvas(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 400);
}

void Canvas::reset()
{
    m_pts.clear();
    m_result = {};
    m_hasResult = false;
    update();
}

void Canvas::setPoints(const QVector<QPointF>& points)
{
    m_pts = points;
    m_hasResult = false;
    update();
}

void Canvas::setResult(const SolverResult& result)
{
    m_result = result;
    m_hasResult = true;
    update();
}

QVector<QPointF> Canvas::fitPoints() const
{
    if (m_pts.isEmpty()) {
        return {};
    }

    qreal minX = m_pts.front().x();
    qreal maxX = m_pts.front().x();
    qreal minY = m_pts.front().y();
    qreal maxY = m_pts.front().y();

    for (const auto& point : m_pts) {
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }

    QRectF src(minX, minY, std::max<qreal>(maxX - minX, 1.0), std::max<qreal>(maxY - minY, 1.0));
    const QRectF dst = rect().adjusted(70, 55, -70, -55);

    QVector<QPointF> fitted;
    fitted.reserve(m_pts.size());
    for (const auto& point : m_pts) {
        fitted.append(mapPoint(point, src, dst));
    }
    return fitted;
}

void Canvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().color(QPalette::Base));

    const auto fittedPoints = fitPoints();
    drawGrid(painter);
    drawMst(painter, fittedPoints);
    drawPoints(painter, fittedPoints);

    if (m_pts.isEmpty()) {
        painter.setPen(palette().color(QPalette::Mid));
        painter.drawText(rect(), Qt::AlignCenter, "Open example.txt to load islands");
    }
}

void Canvas::drawGrid(QPainter& painter)
{
    QColor dotColor = palette().color(QPalette::Mid);
    dotColor.setAlphaF(0.18);
    painter.setPen(Qt::NoPen);
    painter.setBrush(dotColor);

    const int step = 28;
    for (int x = step; x < width(); x += step) {
        for (int y = step; y < height(); y += step) {
            painter.drawEllipse(QPointF(x, y), 1.0, 1.0);
        }
    }
}

void Canvas::drawMst(QPainter& painter, const QVector<QPointF>& fittedPoints)
{
    if (!m_hasResult || m_result.mstEdges.isEmpty()) {
        return;
    }

    QPen pen(palette().color(QPalette::Highlight), 2.5, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    for (const auto& edge : m_result.mstEdges) {
        const int u = edge.first;
        const int v = edge.second;
        if (u >= 0 && u < fittedPoints.size() && v >= 0 && v < fittedPoints.size()) {
            painter.drawLine(fittedPoints[u], fittedPoints[v]);
        }
    }
}

void Canvas::drawPoints(QPainter& painter, const QVector<QPointF>& fittedPoints)
{
    if (fittedPoints.isEmpty()) {
        return;
    }

    QColor ptFill = palette().color(QPalette::Highlight);
    QColor ptBorder = ptFill.lighter(150);
    QColor ptText = palette().color(QPalette::HighlightedText);

    QFont font = painter.font();
    font.setPointSizeF(8.0);
    font.setBold(true);
    painter.setFont(font);

    const qreal radius = 12.0;
    for (int i = 0; i < fittedPoints.size(); ++i) {
        QPointF center = fittedPoints[i];

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 30));
        painter.drawEllipse(center + QPointF(1, 1), radius, radius);

        painter.setBrush(ptFill);
        painter.setPen(QPen(ptBorder, 1.5));
        painter.drawEllipse(center, radius, radius);

        painter.setPen(ptText);
        QRectF labelRect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
        painter.drawText(labelRect, Qt::AlignCenter, QString::number(i));
    }
}

void Canvas::resizeEvent(QResizeEvent*)
{
    update();
}