#include "DynamicGraph.h"

#include <cmath>
#include <utility>

namespace dgraph {

    DynamicGraph::DynamicGraph(int n) : n(n), parent(vector<int>(n, -1)) {
        size = std::lround(std::ceil(std::log2(n)) + 1);
        for (int i = 0; i < size; i++) {
            forests.emplace_back(n);
            adjLists.emplace_back();
            for (int j = 0; j < n; j++){
                adjLists[i].push_back(new List());
            }
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
                // to do that choose less component
                if(forests[i].size(v) > forests[i].size(u)){
                    std::swap(v, u);
                }
                // and iterate over good vertices until success
                Iterator it = forests[i].iterator(v);
                while(it.hasNext()){
                    int w = *it;
                    ListIterator lit = adjLists[i][w]->iterator();
                    while(lit.hasNext()){
                        int up = (*lit)->vertex();
                        if(is_connected(up, u)){
                            for (int j = i; j < size; j++){
                                forests[j].link(w, up);
                                parent[up] = w;
                            }
                            return;
                        } else {
                            downgrade(w, up, (*(lit++))->e());
                        }
                    }
                    it++;
                }
            }
        }
    }

    void DynamicGraph::downgrade(int v, int w, Edge* e){
        int lvl = e->level();
        e->levelDown();
        e->removeLinks();
        e->subscribe(adjLists[lvl - 1][w]->add(v, e));
        e->subscribe(adjLists[lvl - 1][v]->add(w, e));
        forests[lvl].changeEdges(w, -1);
        forests[lvl].changeEdges(v, -1);
        forests[lvl - 1].changeEdges(w, 1);
        forests[lvl - 1].changeEdges(v, 1);
    }

    bool DynamicGraph::is_connected(int v, int u) {
        return forests[forests.size() - 1].is_connected(v, u);
    }

    std::string to_string(DynamicGraph& graph) {
        std::string str;
        for(int i = 0; i < graph.size; i++){
            str += "level " + std::to_string(i) + ": \n";
            str += to_string(graph.forests[i]) + "\n";
        }
        return str;
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
        u = -1;
    }

    ListIterator List::iterator() {
        return ListIterator(next);
    }

    int List::vertex() {
        return u;
    }

    Edge* List::e() {
        return edge;
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

    void Edge::levelDown() {
        lvl--;
    }

    void Edge::removeLinks() {
        for(List* l: links){
            delete l;
        }
        links.clear();
    }

    ListIterator::ListIterator(List* list) :list(list) {}

    ListIterator ListIterator::operator++(int) {
        ListIterator state(list);
        list = list->next;
        return state;
    }

    List* ListIterator::operator*() {
        return list;
    }

    bool ListIterator::hasNext() {
        return list->edge != nullptr;
    }
}
