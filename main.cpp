#include "solver.h"

#include <fstream>
#include <iomanip>
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::ifstream inputFile("example.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file: example.txt\n";
        return 1;
    }

    int caseCount;
    if (!(inputFile >> caseCount)) {
        return 0;
    }

    for (int caseIndex = 0; caseIndex < caseCount; ++caseIndex) {
        int m;
        if (!(inputFile >> m)) {
            std::cerr << "Unexpected end of input while reading case " << caseIndex + 1 << '\n';
            return 1;
        }

        std::vector<Point> points;
        points.reserve(m);

        for (int i = 0; i < m; ++i) {
            double x, y;
            if (!(inputFile >> x >> y)) {
                std::cerr << "Unexpected end of input while reading point " << i + 1
                          << " in case " << caseIndex + 1 << '\n';
                return 1;
            }
            points.push_back({x, y, i});
        }

        SolverResult result = Solver::solve(points);
        std::cout << std::fixed << std::setprecision(10) << result.mstLength << '\n';
    }

    return 0;
}