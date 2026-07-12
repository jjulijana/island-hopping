#include "solver.h"
#include "algorithms.cpp"

SolverResult Solver::solve(const std::vector<Point>& points) {
    std::vector<Point> pts = points;

    for (int i = 0; i < static_cast<int>(pts.size()); ++i) {
        pts[i].id = i;
    }
    
    SolverResult result;
    if (pts.size() == 1) {
        result.mstLength = 0.0;
        return result;
    }

    if (pts.size() == 2) {
        result.delaunayEdges.push_back({0, 1});
        result.mstEdges.push_back({0, 1});
        result.mstLength = dist(pts[0], pts[1]);
        return result;
    }

    auto delEdges = delaunayTriangulation(pts);
    auto [len, mstEdges] = computeMST(pts, delEdges);
    
    for (const auto& [u, v] : delEdges) {
        result.delaunayEdges.push_back({u, v});
    }
        
    for (const auto& [u, v] : mstEdges) {
        result.mstEdges.push_back({u, v});
    }

    result.mstLength = len;
    return result;
}
