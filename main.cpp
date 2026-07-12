#include "solver.h"

#include <iomanip>
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int m;
    if (!(std::cin >> m)) {
        return 0;
    }

    std::vector<Point> points;
    points.reserve(m);

    for (int i = 0; i < m; ++i) {
        double x, y;
        std::cin >> x >> y;
        points.push_back({x, y, i});
    }

    SolverResult result = Solver::solve(points);

    std::cout << std::fixed << std::setprecision(10) << result.mstLength << '\n';
    return 0;
}