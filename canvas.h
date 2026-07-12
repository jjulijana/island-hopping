#pragma once

#include "solver.h"

#include <QPointF>
#include <QVector>
#include <QWidget>

class QPainter;

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget* parent = nullptr);

    void reset();
    void setPoints(const QVector<QPointF>& points);
    void setResult(const SolverResult& result);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QVector<QPointF> fitPoints() const;
    void drawGrid(QPainter& painter);
    void drawMst(QPainter& painter, const QVector<QPointF>& fittedPoints);
    void drawPoints(QPainter& painter, const QVector<QPointF>& fittedPoints);

    QVector<QPointF> m_pts;
    SolverResult m_result;
    bool m_hasResult = false;
};