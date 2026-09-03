#pragma once
#include <cmath>

struct Point {
    double x, y;
    int id;
};

inline double dist2(const Point& a, const Point& b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx*dx + dy*dy;
}

inline double dist(const Point& a, const Point& b) {
    return std::sqrt(dist2(a, b));
}