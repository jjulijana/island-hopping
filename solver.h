#pragma once
#include "components.h"
#include <utility>
#include <vector>

struct SolverResult {
    std::vector<std::pair<int, int>> delaunayEdges;
    std::vector<std::pair<int, int>> mstEdges;
    double mstLength = 0.0;
}; 

class Solver
{

public: 
    static SolverResult solve(const std::vector<Point>& points); 

}; 
