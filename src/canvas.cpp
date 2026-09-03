#include "canvas.h"

#include <algorithm>
#include <cmath>

#include <QFontMetricsF>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QWheelEvent>

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

qreal pointDistance(const QPointF& a, const QPointF& b)
{
    const qreal dx = a.x() - b.x();
    const qreal dy = a.y() - b.y();
    return std::sqrt(dx * dx + dy * dy);
}

QRectF centeredUniformRect(const QRectF& src, const QRectF& dst)
{
    if (src.width() <= 0.0 || src.height() <= 0.0 || dst.width() <= 0.0 || dst.height() <= 0.0) {
        return dst;
    }

    const qreal scale = std::min(dst.width() / src.width(), dst.height() / src.height());
    const qreal w = src.width() * scale;
    const qreal h = src.height() * scale;

    QRectF fitted(0, 0, w, h);
    fitted.moveCenter(dst.center());
    return fitted;
}

void drawLengthLabel(QPainter& painter, const QPointF& mid, double length)
{
    const QString lengthText = QString::number(length, 'f', 1);

    QFont labelFont = painter.font();
    labelFont.setPointSizeF(8.0);
    labelFont.setBold(true);
    painter.setFont(labelFont);

    QFontMetricsF metrics(labelFont);
    const QRectF textRect = metrics.boundingRect(lengthText);
    QRectF bubbleRect = textRect.adjusted(-4, -2, 4, 2);
    bubbleRect.moveCenter(mid + QPointF(0, -10));

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 215));
    painter.drawRoundedRect(bubbleRect, 3, 3);

    painter.setPen(Qt::black);
    painter.drawText(bubbleRect, Qt::AlignCenter, lengthText);
}
}

Canvas::Canvas(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 400);
    m_playTimer.setSingleShot(false);
    connect(&m_playTimer, &QTimer::timeout, this, &Canvas::advancePlayback);
}

void Canvas::resetVisualizationState()
{
    m_phase = Idle;
    m_finalOnly = false;
    m_visibleDelaunayEdges = 0;
    m_visibleMstEdges = 0;
    m_playbackStarted = false;
    m_playTimer.stop();
}

void Canvas::setPoints(const QVector<QPointF>& points)
{
    resetVisualizationState();
    m_zoomFactor = 1.0;
    m_panOffset = {};
    m_pts = points;
    m_hasResult = false;
    update();
    emit stateChanged();
}

void Canvas::setResult(const SolverResult& result)
{
    resetVisualizationState();
    m_result = result;
    m_hasResult = true;
    update();
    emit stateChanged();
}

void Canvas::showDelaunay()
{
    if (!m_hasResult || m_pts.isEmpty()) {
        return;
    }

    m_playTimer.stop();
    m_playbackStarted = false;
    setPhase(ShowDelaunay);
    m_visibleDelaunayEdges = m_result.delaunayEdges.size();
    m_visibleMstEdges = 0;
    emit stateChanged();
}

void Canvas::showMstAll()
{
    if (!m_hasResult || m_pts.isEmpty()) {
        return;
    }

    m_playTimer.stop();
    m_playbackStarted = false;
    m_finalOnly = true;
    setPhase(ShowMst);
    m_visibleDelaunayEdges = m_result.delaunayEdges.size();
    update();
    emit stateChanged();
}

void Canvas::showMstStepByStep()
{
    if (!m_hasResult || m_pts.isEmpty()) {
        return;
    }

    m_playTimer.stop();
    m_playbackStarted = false;
    m_finalOnly = false;
    setPhase(ShowMst);
    m_visibleDelaunayEdges = m_result.delaunayEdges.size();
    m_visibleMstEdges = 0;
    update();
    emit stateChanged();
}

void Canvas::playStepByStep()
{
    if (!m_hasResult || m_pts.isEmpty()) {
        return;
    }

    if (!m_playbackStarted) {
        m_finalOnly = false;
        setPhase(ShowDelaunay);
        m_visibleDelaunayEdges = 0;
        m_visibleMstEdges = 0;
        m_playbackStarted = true;
    }

    m_playTimer.start(m_stepIntervalMs);
    update();
    emit stateChanged();
}

void Canvas::nextMstEdge()
{
    if (m_phase != ShowMst || !m_hasResult || m_finalOnly) {
        return;
    }

    if (m_visibleMstEdges < m_result.mstConsideredEdges.size()) {
        ++m_visibleMstEdges;
        update();
        emit stateChanged();
    }
}

void Canvas::pausePlayback()
{
    m_playTimer.stop();
    emit stateChanged();
}

void Canvas::setStepIntervalMs(int ms)
{
    m_stepIntervalMs = std::max(50, ms);
    if (m_playTimer.isActive()) {
        m_playTimer.start(m_stepIntervalMs);
    }
}

int Canvas::pointCount() const
{
    return m_pts.size();
}

int Canvas::visibleDelaunayEdges() const
{
    return m_visibleDelaunayEdges;
}

int Canvas::delaunayEdgeCount() const
{
    return m_result.delaunayEdges.size();
}

int Canvas::visibleMstEdges() const
{
    return m_visibleMstEdges;
}

int Canvas::mstConsideredEdgeCount() const
{
    return m_result.mstConsideredEdges.size();
}

int Canvas::mstEdgeCount() const
{
    return m_result.mstEdges.size();
}

double Canvas::mstLength() const
{
    return m_result.mstLength;
}

qreal Canvas::zoomFactor() const
{
    return m_zoomFactor;
}

QPointF Canvas::panOffset() const
{
    return m_panOffset;
}

bool Canvas::isPlaying() const
{
    return m_playTimer.isActive();
}

Canvas::Phase Canvas::phase() const
{
    return m_phase;
}

void Canvas::resetZoom()
{
    m_zoomFactor = 1.0;
    m_panOffset = {};
    update();
    emit stateChanged();
}

void Canvas::setPhase(Phase phase)
{
    m_phase = phase;
    update();
    emit stateChanged();
}

void Canvas::advancePlayback()
{
    if (!m_hasResult) {
        m_playTimer.stop();
        return;
    }

    if (m_phase == ShowDelaunay) {
        if (m_visibleDelaunayEdges < m_result.delaunayEdges.size()) {
            ++m_visibleDelaunayEdges;
            update();
            emit stateChanged();
            return;
        }

        setPhase(ShowMst);
        m_visibleMstEdges = 0;
        return;
    }

    if (m_phase == ShowMst) {
        if (m_visibleMstEdges < m_result.mstConsideredEdges.size()) {
            ++m_visibleMstEdges;
            update();
            emit stateChanged();
        } else {
            m_playTimer.stop();
            m_playbackStarted = false;
            emit stateChanged();
        }
    }
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
    const QRectF available = rect().adjusted(70, 55, -70, -55);
    const QRectF dst = centeredUniformRect(src, available);

    QVector<QPointF> fitted;
    fitted.reserve(m_pts.size());
    const QPointF center = rect().center();
    for (const auto& point : m_pts) {
        const QPointF mapped = mapPoint(point, src, dst);
        fitted.append(center + (mapped - center) * m_zoomFactor + m_panOffset);
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
    if (m_phase == ShowDelaunay) {
        drawDelaunay(painter, fittedPoints);
    } else if (m_phase == ShowMst) {
        drawDelaunay(painter, fittedPoints);
        if (m_finalOnly) {
            drawFinalMst(painter, fittedPoints);
        } else {
            drawMst(painter, fittedPoints);
        }
    }
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

void Canvas::drawDelaunay(QPainter& painter, const QVector<QPointF>& fittedPoints)
{
    if (!m_hasResult || m_result.delaunayEdges.isEmpty()) {
        return;
    }

    QPen pen(QColor(120, 120, 120, 170), 1.6, Qt::DashLine, Qt::RoundCap);
    pen.setDashPattern({5, 4});
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    const int visibleCount = (m_phase == ShowDelaunay || m_phase == ShowMst)
        ? m_visibleDelaunayEdges
        : 0;

    for (int i = 0; i < visibleCount && i < m_result.delaunayEdges.size(); ++i) {
        const auto& edge = m_result.delaunayEdges[i];
        const int u = edge.first;
        const int v = edge.second;
        if (u >= 0 && u < fittedPoints.size() && v >= 0 && v < fittedPoints.size()) {
            painter.drawLine(fittedPoints[u], fittedPoints[v]);
        }
    }
}

void Canvas::drawMst(QPainter& painter, const QVector<QPointF>& fittedPoints)
{
    if (!m_hasResult || m_result.mstConsideredEdges.isEmpty()) {
        return;
    }

    const int visibleCount = std::min(m_visibleMstEdges, static_cast<int>(m_result.mstConsideredEdges.size()));

    QPen whitePen(QColor(255, 255, 255, 180), 1.8, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(whitePen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 0; i < visibleCount; ++i) {
        const auto& edge = m_result.mstConsideredEdges[i];
        const int u = edge.first;
        const int v = edge.second;
        if (u >= 0 && u < fittedPoints.size() && v >= 0 && v < fittedPoints.size()) {
            painter.drawLine(fittedPoints[u], fittedPoints[v]);
        }
    }

    QPen greenGlow(QColor(30, 180, 70, 110), 6.0, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(greenGlow);

    for (int i = 0; i < visibleCount; ++i) {
        if (!m_result.mstConsideredAccepted[i]) {
            continue;
        }

        const auto& edge = m_result.mstConsideredEdges[i];
        const int u = edge.first;
        const int v = edge.second;
        if (u >= 0 && u < fittedPoints.size() && v >= 0 && v < fittedPoints.size()) {
            const QPointF a = fittedPoints[u];
            const QPointF b = fittedPoints[v];
            painter.drawLine(a, b);

            QPen greenPen(QColor(30, 180, 70), 3.0, Qt::SolidLine, Qt::RoundCap);
            painter.setPen(greenPen);
            painter.drawLine(a, b);

            const QPointF mid = (a + b) / 2.0;
            const double treeLength = (i < m_result.mstConsideredEdgeLengths.size())
                ? m_result.mstConsideredEdgeLengths[i]
                : pointDistance(a, b);
            drawLengthLabel(painter, mid, treeLength);

            painter.setPen(greenGlow);
        }
    }
}

void Canvas::drawFinalMst(QPainter& painter, const QVector<QPointF>& fittedPoints)
{
    if (!m_hasResult || m_result.mstEdges.isEmpty()) {
        return;
    }

    QPen greenGlow(QColor(30, 180, 70, 110), 6.0, Qt::SolidLine, Qt::RoundCap);
    QPen greenPen(QColor(30, 180, 70), 3.0, Qt::SolidLine, Qt::RoundCap);

    for (int i = 0; i < m_result.mstEdges.size(); ++i) {
        const auto& edge = m_result.mstEdges[i];
        const int u = edge.first;
        const int v = edge.second;
        if (u < 0 || u >= fittedPoints.size() || v < 0 || v >= fittedPoints.size()) {
            continue;
        }

        const QPointF a = fittedPoints[u];
        const QPointF b = fittedPoints[v];

        painter.setPen(greenGlow);
        painter.drawLine(a, b);

        painter.setPen(greenPen);
        painter.drawLine(a, b);

        const QPointF mid = (a + b) / 2.0;
        const double treeLength = (i < m_result.mstEdgeLengths.size())
            ? m_result.mstEdgeLengths[i]
            : pointDistance(a, b);
        drawLengthLabel(painter, mid, treeLength);
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

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastPanPosition = event->position();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    event->ignore();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_isPanning) {
        event->ignore();
        return;
    }

    const QPointF currentPosition = event->position();
    m_panOffset += currentPosition - m_lastPanPosition;
    m_lastPanPosition = currentPosition;
    update();
    emit stateChanged();
    event->accept();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPanning) {
        m_isPanning = false;
        unsetCursor();
        event->accept();
        return;
    }

    event->ignore();
}

void Canvas::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() == 0) {
        event->ignore();
        return;
    }

    const qreal zoomStep = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    m_zoomFactor = std::clamp(m_zoomFactor * zoomStep, 0.5, 20.0);
    update();
    emit stateChanged();
    event->accept();
}