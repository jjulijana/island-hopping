#pragma once

#include "solver.h"

#include <QPointF>
#include <QVector>
#include <QWidget>
#include <QTimer>

class QPainter;

class Canvas : public QWidget
{
    Q_OBJECT

public:
    enum Phase { Idle, ShowDelaunay, ShowMst };

    explicit Canvas(QWidget* parent = nullptr);

    void reset();
    void setPoints(const QVector<QPointF>& points);
    void setResult(const SolverResult& result);
    void showDelaunay();
    void showMstAll();
    void showMstStepByStep();
    void playStepByStep();
    void nextMstEdge();
    void showAllMstEdges();
    void pausePlayback();
    void setStepIntervalMs(int ms);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QVector<QPointF> fitPoints() const;
    void drawGrid(QPainter& painter);
    void drawDelaunay(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void drawMst(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void drawPoints(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void setPhase(Phase phase);
    void advancePlayback();

    QVector<QPointF> m_pts;
    SolverResult m_result;
    bool m_hasResult = false;
    Phase m_phase = Idle;
    int m_visibleDelaunayEdges = 0;
    int m_visibleMstEdges = 0;
    int m_stepIntervalMs = 500;
    QTimer m_playTimer;
};