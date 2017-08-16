#include "DynamicGraph.h"

#include <cmath>

namespace dgraph {

    DynamicGraph::DynamicGraph(int n) : n(n) {
        long size = std::lround(std::ceil(std::log2(n)) + 1);
        for (long i = 0; i < size; i++) {
            forests.emplace_back(n);
        }
    }

    Edge* DynamicGraph::add(int v, int u) {
        auto* edge = new Edge(static_cast<int>(forests.size() - 1));
        if (!is_connected(v, u)) {
            forests[forests.size() - 1].link(v, u);
        }
    }

    void DynamicGraph::remove(Edge* link) {

    }

    bool DynamicGraph::is_connected(int v, int u) {
        return forests[forests.size() - 1].is_connected(v, u);
    }

    AdjList::AdjList(int n) {
        for (int i = 0; i < n; i++) {
            adj.emplace_back();
        }
    }
}
