#include "DynamicGraph.h"

#include <cmath>

namespace dgraph {

    DynamicGraph::DynamicGraph(int n) : n(n), parent(vector<int>(n, -1)) {
        size = std::lround(std::ceil(std::log2(n)) + 1);
        for (int i = 0; i < size; i++) {
            forests.emplace_back(n);
            adjLists.emplace_back(n, new List());
        }
    }

    Edge* DynamicGraph::add(int v, int u) {
        auto n = forests.size() - 1;
        auto* edge = new Edge(n);
        if (!is_connected(v, u)) {
            forests[n].link(v, u);
            parent[u] = v;
        }
        forests[n].changeEdges(v, 1);
        forests[n].changeEdges(u, 1);
        edge->subscribe(adjLists[n][v]->add(u, edge));
        edge->subscribe(adjLists[n][u]->add(v, edge));
        return edge;
    }

    void DynamicGraph::remove(Edge* link) {
        int u = link->where();
        int level = link->level();
        delete link;
        int v = parent[u];
        if (v != -1){
            parent[u] = -1;
            for (int i = level; i < size; i++){
                forests[i].cut(u);
            }
            for (int i = level; i < size; i++){
                // find new connection

            }
        }
    }

    bool DynamicGraph::is_connected(int v, int u) {
        return forests[forests.size() - 1].is_connected(v, u);
    }

    List* List::add(int v, Edge* edge) {
        auto* newList = new List(v, edge, this, next);
        next->prev = newList;
        next = newList;
        return newList;
    }

    List::~List() {
        next->prev = prev;
        prev->next = next;
    }

    List::List(int u, Edge* edge, List* prev, List* next) :u(u), edge(edge), prev(prev), next(next){}

    List::List() {
        next = this;
        prev = this;
    }

    Edge::Edge(unsigned lvl) : lvl(lvl) {}

    void Edge::subscribe(List* link) {
        links.push_back(link);
    }

    Edge::~Edge() {
        for (List* link : links){
            delete link;
        }
    }

    int Edge::where() {
        return links[0]->u;
    }

    unsigned Edge::level() {
        return lvl;
    }
}
