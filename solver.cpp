#include "solver.h"
#include "algorithms.cpp"

#include <vector>

SolverResult Solver::solve(const QVector<QPointF>& points) {
    std::vector<Point> pts;
    pts.reserve(points.size());

    for (int i = 0; i < points.size(); ++i) {
        pts.push_back({points[i].x(), points[i].y(), i});
    }

    SolverResult result;
    if (pts.size() == 1) {
        result.mstLength = 0.0;
        return result;
    }

    if (pts.size() == 2) {
        result.delaunayEdges.append({0, 1});
        result.mstEdges.append({0, 1});
        result.mstLength = dist(pts[0], pts[1]);
        return result;
    }

    auto delEdges = delaunayTriangulation(pts);
    auto [len, mstEdges] = computeMST(pts, delEdges);
    
    for (const auto& [u, v] : delEdges) {
        result.delaunayEdges.append({u, v});
    }
        
    for (const auto& [u, v] : mstEdges) {
        result.mstEdges.append({u, v});
    }

    result.mstLength = len;
    return result;
}
