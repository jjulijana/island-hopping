#pragma once

#include <QPair>
#include <QPointF>
#include <QVector>

struct SolverResult {
    QVector<QPair<int, int>> delaunayEdges;
    QVector<QPair<int, int>> mstConsideredEdges;
    QVector<double> mstConsideredEdgeLengths;
    QVector<bool> mstConsideredAccepted;
    QVector<QPair<int, int>> mstEdges;
    QVector<double> mstEdgeLengths;
    double mstLength = 0.0;
};

namespace Solver {
    SolverResult solve(const QVector<QPointF>& points);
}