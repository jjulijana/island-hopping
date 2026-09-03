#pragma once

#include "solver.h"

#include <QPointF>
#include <QVector>
#include <QWidget>
#include <QTimer>

class QMouseEvent;
class QPainter;
class QWheelEvent;

class Canvas : public QWidget
{
    Q_OBJECT

public:
    enum Phase { Idle, ShowDelaunay, ShowMst };

    explicit Canvas(QWidget* parent = nullptr);

    void setPoints(const QVector<QPointF>& points);
    void setResult(const SolverResult& result);
    void showDelaunay();
    void showMstAll();
    void showMstStepByStep();
    void playStepByStep();
    void nextMstEdge();
    void pausePlayback();
    void setStepIntervalMs(int ms);
    void resetZoom();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QVector<QPointF> fitPoints() const;
    void drawGrid(QPainter& painter);
    void drawDelaunay(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void drawMst(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void drawFinalMst(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void drawPoints(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void resetVisualizationState();
    void setPhase(Phase phase);
    void advancePlayback();

    QVector<QPointF> m_pts;
    SolverResult m_result;
    bool m_hasResult = false;
    Phase m_phase = Idle;
    bool m_finalOnly = false;
    int m_visibleDelaunayEdges = 0;
    int m_visibleMstEdges = 0;
    int m_stepIntervalMs = 500;
    qreal m_zoomFactor = 1.0;
    QPointF m_panOffset;
    QPointF m_lastPanPosition;
    bool m_isPanning = false;
    QTimer m_playTimer;
};