#include "components.h"
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef Delaunay::Vertex_handle Vertex_handle;

vector<pair<int, int>> delaunayTriangulation(const vector<Point>& points) {
    Delaunay dt;
    map<Vertex_handle, int> vh_to_id;
   
    for (const auto& p : points) {
        auto vh = dt.insert(K::Point_2(p.x, p.y)); // Specifican format za CGAL, K je kernel, definise aritmetiku
        vh_to_id[vh] = p.id;
    }

    set<pair<int, int>> edgeSet; // Izbegavamo duplikate, prebacujemo u citljiv format
    for (auto eit = dt.finite_edges_begin(); eit != dt.finite_edges_end(); ++eit) {
        auto face = eit->first;
        int index = eit->second;

        auto vh1 = face->vertex((index + 1) % 3);
        auto vh2 = face->vertex((index + 2) % 3);

        if (dt.is_infinite(vh1) || dt.is_infinite(vh2)) {
            continue;
        }

        int id1 = vh_to_id[vh1];
        int id2 = vh_to_id[vh2];
        if (id1 > id2) {
            swap(id1, id2); // Rastuci redosled
        }
        edgeSet.insert({id1, id2});
    }

    return vector<pair<int, int>>(edgeSet.begin(), edgeSet.end());
}

struct UnionFind {
    vector<int> parent, rank_;

    explicit UnionFind(int n) : parent(n), rank_(n, 0) {
        for (int i = 0; i < n; i++) { // Inicijalizacija svakog elementa da bude svoj roditelj
            parent[i] = i;
        }
    }

    int find(int x) { // Path compression optimizacija
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    bool unite(int x, int y) { // Unija po ranku
        int rx = find(x), ry = find(y);
        if (rx == ry) {
            return false; // Vec su u istoj komonenti
        }
        if (rank_[rx] < rank_[ry]) {
            swap(rx, ry);
        }
        parent[ry] = rx; // Roditelj elementa manjeg ranka postaje element vecceg ranka
        if (rank_[rx] == rank_[ry]) {
            rank_[rx]++;
        }
        return true;
    }
};

struct MstComputation {
    double totalLength = 0.0;
    vector<pair<int, int>> consideredEdges;
    vector<double> consideredLengths;
    vector<bool> consideredAccepted;
    vector<pair<int, int>> mstEdges;
    vector<double> mstEdgeLengths;
};

MstComputation computeMST(const vector<Point>& points, const vector<pair<int, int>>& delaunayEdges) {
    int n = static_cast<int>(points.size());
    vector<tuple<double, int, int>> edges;
    edges.reserve(delaunayEdges.size());

    for (const auto& [u, v] : delaunayEdges) {
        edges.emplace_back(dist(points[u], points[v]), u, v);
    }

    sort(edges.begin(), edges.end()); // Sortirane po tezini (distanci)

    UnionFind uf(n);
    MstComputation computation;
    computation.consideredEdges.reserve(edges.size());
    computation.consideredLengths.reserve(edges.size());
    computation.consideredAccepted.reserve(edges.size());
    computation.mstEdges.reserve(max(0, n - 1));
    computation.mstEdgeLengths.reserve(max(0, n - 1));

    int edgesUsed = 0;
    for (auto& [w, u, v] : edges) {
        computation.consideredEdges.push_back({u, v});
        computation.consideredLengths.push_back(w);
        bool accepted = false;
        if (uf.unite(u, v)) { // Ako su u različitim komponentama, spojimo ih
            computation.totalLength += w;
            accepted = true;
            if (u > v) {
                swap(u, v); // Rastuci redosled
            }
            computation.mstEdges.push_back({u, v});
            computation.mstEdgeLengths.push_back(w);
            edgesUsed++;
            if (edgesUsed == n - 1) {
                computation.consideredAccepted.push_back(accepted);
                break; // Iskoristili sve
            }
        }
        computation.consideredAccepted.push_back(accepted);
    }

    return computation;
}
