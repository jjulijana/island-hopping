#include <QApplication>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "mainwindow.h"

namespace {

int runConsoleMode(const char* inputPath) {
    std::ifstream inputFile(inputPath);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputPath << '\n';
        return 1;
    }

    int caseCount = 0;
    if (!(inputFile >> caseCount)) {
        return 0;
    }

    for (int caseIndex = 0; caseIndex < caseCount; ++caseIndex) {
        int pointCount = 0;
        if (!(inputFile >> pointCount)) {
            std::cerr << "Unexpected end of input while reading case " << caseIndex + 1 << '\n';
            return 1;
        }

        QVector<QPointF> points;
        points.reserve(pointCount);

        for (int i = 0; i < pointCount; ++i) {
            double x = 0.0;
            double y = 0.0;
            if (!(inputFile >> x >> y)) {
                std::cerr << "Unexpected end of input while reading point " << i + 1
                          << " in case " << caseIndex + 1 << '\n';
                return 1;
            }
            points.append(QPointF(x, y));
        }

        const SolverResult result = Solver::solve(points);
        std::cout << std::fixed << std::setprecision(10) << result.mstLength << '\n';
    }

    return 0;
}
}

int main(int argc, char* argv[])
{
    const bool consoleMode = argc >= 2 && std::string(argv[1]) == "--console";

    if (consoleMode) {
        const char* inputPath = (argc >= 3) ? argv[2] : "example.txt";
        return runConsoleMode(inputPath);
    }

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}