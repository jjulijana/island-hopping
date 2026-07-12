#pragma once
#include "components.h"

#include <QPair>
#include <QPointF>
#include <QVector>

struct SolverResult {
    QVector<QPair<int, int>> delaunayEdges;
    QVector<QPair<int, int>> mstEdges;
    double mstLength = 0.0;
}; 

class Solver
{

public: 
    static SolverResult solve(const QVector<QPointF>& points); 

}; 
