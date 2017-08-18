#include "DynamicGraph.h"

#include <cmath>

namespace dgraph {

    DynamicGraph::DynamicGraph(int n) : n(n) {
        long size = std::lround(std::ceil(std::log2(n)) + 1);
        for (long i = 0; i < size; i++) {
            forests.emplace_back(n);
            adjLists.emplace_back();
        }
    }

    Edge* DynamicGraph::add(int v, int u) {
        auto n = forests.size() - 1;
        auto* edge = new Edge(n);
        if (!is_connected(v, u)) {
            forests[n].link(v, u);
        }
        List* entry = adjLists[n][v]->add(u, edge);
        edge->subscribe(entry);
        return edge;
    }

    void DynamicGraph::remove(Edge* link) {

    }

    bool DynamicGraph::is_connected(int v, int u) {
        return forests[forests.size() - 1].is_connected(v, u);
    }

    List* List::add(int v, Edge* edge) {
        auto* newList = new List(v, edge, this, next);
        if (next != nullptr){
            next->prev = newList;
        }
        next = newList;
        return newList;
    }

    void List::remove() {
        if(next != nullptr){
            next->prev = prev;
        }
        prev->next = next;
        delete this;
    }

    List::List(int u, Edge* edge, List* prev, List* next) :u(u), edge(edge), prev(prev), next(next){}
}
