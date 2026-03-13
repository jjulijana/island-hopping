#include <bits/stdc++.h>
using namespace std;

const double EPS = EPS;

struct Point {
    double x, y;
    int id;
};

inline double dist2(const Point& a, const Point& b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx*dx + dy*dy;
}

inline double dist(const Point& a, const Point& b) {
    return sqrt(dist2(a, b));
}

struct Edge {
    Point p, q;
};

inline bool edgesEqual(const Edge& e1, const Edge& e2) {
    return (fabs(e1.p.x - e2.p.x) < EPS && fabs(e1.p.y - e2.p.y) < EPS &&
            fabs(e1.q.x - e2.q.x) < EPS && fabs(e1.q.y - e2.q.y) < EPS) ||
           (fabs(e1.p.x - e2.q.x) < EPS && fabs(e1.p.y - e2.q.y) < EPS &&
            fabs(e1.q.x - e2.p.x) < EPS && fabs(e1.q.y - e2.p.y) < EPS);
}