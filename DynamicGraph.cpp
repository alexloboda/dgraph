#include "DynamicGraph.h"

#include <cmath>
#include <utility>

namespace dgraph {

    DynamicGraph::DynamicGraph(unsigned n) : n(n) {
        size = std::lround(std::ceil(std::log2(n)) + 1);
        for (unsigned i = 0; i < size; i++) {
            forests.emplace_back(n);
            adjLists.emplace_back();
            for (unsigned j = 0; j < n; j++){
                adjLists[i].push_back(new List());
            }
        }
    }

    Edge* DynamicGraph::add(unsigned v, unsigned u) {
        unsigned n = size - 1;
        auto* edge = new Edge(n, v, u);
        if (!is_connected(v, u)) {
            edge->add_tree_edge(forests[n].link(v, u));
        }
        forests[n].changeEdges(v, 1);
        forests[n].changeEdges(u, 1);
        edge->subscribe(adjLists[n][v]->add(u, edge));
        edge->subscribe(adjLists[n][u]->add(v, edge));
        return edge;
    }

    void DynamicGraph::remove(Edge* link) {
        unsigned v = link->from();
        unsigned u = link->to();
        bool complex_deletion = link->is_tree_edge();
        unsigned level = link->level();

        if (complex_deletion) {
            for (unsigned i = 0; i <= size - level; i++){
                forests[size - i - 1].cut(std::move(link->tree_edges[i]));
            }
        }

        delete link;

        if (complex_deletion) {
            for (unsigned i = level; i < size; i++){
                // find new connection
                // to do that choose less component
                if(forests[i].size(v) > forests[i].size(u)){
                    std::swap(v, u);
                }
                // and iterate over good vertices until success
                Iterator it = forests[i].iterator(v);
                while(it.hasNext()){
                    unsigned w = *it;
                    ListIterator lit = adjLists[i][w]->iterator();
                    while(lit.hasNext()){
                        unsigned up = (*lit)->vertex();
                        if(is_connected(up, u)){
                            for (unsigned j = i; j < size; j++){
                                (*lit)->e()->add_tree_edge(forests[j].link(w, up));
                            }
                            return;
                        } else {
                            downgrade(w, up, (*(lit++))->e());
                        }
                    }
                    ++it;
                }
            }
        }
    }

    void DynamicGraph::downgrade(Edge* e){
        unsigned lvl = e->lvl--;
        e->removeLinks();
        e->subscribe(adjLists[lvl - 1][w]->add(v, e));
        e->subscribe(adjLists[lvl - 1][v]->add(w, e));
        forests[lvl].changeEdges(w, -1);
        forests[lvl].changeEdges(v, -1);
        forests[lvl - 1].changeEdges(w, 1);
        forests[lvl - 1].changeEdges(v, 1);
        if (e->is_tree_edge()) {
            forests[lvl - 1].link(v, w);
        }
    }

    bool DynamicGraph::is_connected(unsigned v, unsigned u) {
        return forests[forests.size() - 1].is_connected(v, u);
    }

    std::string to_string(DynamicGraph& graph) {
        std::string str;
        for(unsigned i = 0; i < graph.size; i++){
            str += "level " + std::to_string(i) + ": \n";
            str += to_string(graph.forests[i]) + "\n";
        }
        return str;
    }

    List* List::add(unsigned v, Edge* edge) {
        auto* newList = new List(v, edge, this, next);
        next->prev = newList;
        next = newList;
        return newList;
    }

    List::~List() {
        next->prev = prev;
        prev->next = next;
    }

    List::List(unsigned u, Edge* edge, List* prev, List* next) :u(u), edge(edge), prev(prev), next(next){}

    List::List() {
        next = this;
        prev = this;
        u = -1;
    }

    ListIterator List::iterator() {
        return ListIterator(next);
    }

    unsigned List::vertex() {
        return u;
    }

    Edge* List::e() {
        return edge;
    }

    Edge::Edge(unsigned lvl, unsigned v, unsigned u) : lvl(lvl) {}

    void Edge::subscribe(List* link) {
        links.push_back(link);
    }

    Edge::~Edge() {
        for (List* link : links){
            delete link;
        }
    }

    unsigned Edge::level() {
        return lvl;
    }

    void Edge::removeLinks() {
        for(List* l: links){
            delete l;
        }
        links.clear();
    }

    unsigned Edge::from() {
        return v;
    }

    unsigned Edge::to() {
        return u;
    }

    void Edge::add_tree_edge(TreeEdge&& edge) {
        tree_edges.push_back(std::move(edge));
    }

    bool Edge::is_tree_edge() {
        return !tree_edges.empty();
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
