#include "DynamicGraph.h"

#include <cmath>
#include <utility>
#include <limits>
#include <iostream>

namespace dgraph {
    void swap(vector<Edge*>& adj, unsigned i, unsigned j) {
        if (i != j) {
            std::swap(adj[i], adj[j]);
            adj[i]->change_pos(j, i);
            adj[j]->change_pos(i, j);
        }
    }

    DynamicGraph::DynamicGraph(unsigned n) : n(n) {
        size = std::lround(std::ceil(std::log2(n)) + 1);
        for (unsigned i = 0; i < size; i++) {
            forests.emplace_back(n);
            adjLists.emplace_back();
            threshold.emplace_back();
            for (unsigned j = 0; j < n; j++){
                adjLists[i].emplace_back();
                threshold[i].push_back(0);

            }
        }
    }

    DynamicGraph::~DynamicGraph() {
        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < n; j++) {
                for (Edge* e : adjLists[i][j]) {
                    delete e;
                }
            }
        }
    }

    void DynamicGraph::add_links(Edge* e) {
        unsigned v = e->v;
        unsigned u = e->u;
        unsigned n = e->lvl;
        unsigned vs = adjLists[n][v].size();
        unsigned us = adjLists[n][u].size();
        e->subscribe(vs, us);
        adjLists[n][v].push_back(e);
        adjLists[n][u].push_back(e);
        if (e->is_tree_edge()) {
            // tree edges should be in front of adj list
            swap(adjLists[n][v], vs, threshold[n][v]++);
            swap(adjLists[n][u], us, threshold[n][u]++);
        }

    }

    EdgeToken DynamicGraph::add(unsigned v, unsigned u) {
        if (v == u) {
            return EdgeToken(nullptr);
        }
        unsigned n = size - 1;
        auto* edge = new Edge(n, v, u);
        add_links(edge);
        forests[n].increment_edges(v);
        forests[n].increment_edges(u);
        if (!is_connected(v, u)) {
            edge->add_tree_edge(forests[n].link(v, u));
        }
        return EdgeToken(edge);
    }

    void DynamicGraph::remove(EdgeToken&& edge_token) {
        Edge* link = edge_token.edge;
        if (link == nullptr) {
            return;
        }
        unsigned v = link->from();
        unsigned u = link->to();
        bool complex_deletion = link->is_tree_edge();
        unsigned level = link->level();

        if (complex_deletion) {
            for (unsigned i = 0; i <= size - level - 1; i++){
                forests[size - i - 1].cut(std::move(link->tree_edges[i]));
            }
        }

        forests[level].decrement_edges(v);
        forests[level].decrement_edges(u);

        remove_links(link);

        if (complex_deletion) {
            for (unsigned i = level; i < size; i++){
                // find new connection
                // to do that choose less component
                if(forests[i].size(v) > forests[i].size(u)){
                    std::swap(v, u);
                }
                // and iterate over good vertices until success
                // propagating all tree edges of smallest component
                Edge* replacement = nullptr;
                Iterator it = forests[i].iterator(v);
                while(it.hasNext()){
                    unsigned w = *it;
                    auto& list = adjLists[i][w];
                    for (unsigned j = 0; j < list.size(); j++) {
                        if (replacement != nullptr && i == threshold[i][w]) {
                            break;
                        }
                        Edge* e = adjLists[i][w][j];
                        unsigned up = e->opposite(w);
                        if (is_connected(up, u)) {
                            replacement = e;
                        } else {
                            downgrade(e);
                            j--;
                        }
                    }
                    ++it;
                }

                if (replacement != nullptr) {
                    // Make tree edge be in the right place in adj
                    unsigned v = replacement->v;
                    unsigned u = replacement->u;
                    swap(adjLists[i][v], replacement->first_pos, threshold[i][v]++);
                    swap(adjLists[i][u], replacement->second_pos, threshold[i][u]++);
                    for (unsigned j = size - 1; j >= i; j--){
                        replacement->add_tree_edge(forests[j].link(replacement->v, replacement->u));
                    }
                    break;
                }
            }
        }
    }

    void DynamicGraph::downgrade(Edge* e){
        unsigned v = e->from();
        unsigned w = e->to();
        remove_links(e);
        unsigned lvl = e->lvl--;
        add_links(e);
        forests[lvl].decrement_edges(w);
        forests[lvl].decrement_edges(v);
        forests[lvl - 1].increment_edges(w);
        forests[lvl - 1].increment_edges(v);
        if (e->is_tree_edge()) {
            e->add_tree_edge(forests[lvl - 1].link(v, w));
        }
    }

    bool DynamicGraph::is_connected(unsigned v, unsigned u) {
        return forests[forests.size() - 1].is_connected(v, u);
    }

    bool DynamicGraph::is_connected() {
        return forests[forests.size() - 1].is_connected();
    }

    std::string DynamicGraph::str() {
        std::string str;
        for(unsigned i = 0; i < size; i++){
            str += "level " + std::to_string(i) + ": \n";
            str += forests[i].str() + "\n";
        }
        return str;
    }

    unsigned DynamicGraph::degree(unsigned v) {
        unsigned sum = 0;
        for (unsigned i = 0; i < size; i++) {
            sum += forests[i].degree(v);
        }
        return sum;
    }

    void DynamicGraph::remove_links(Edge* e) {
        unsigned v = e->v;
        unsigned u = e->u;
        unsigned lvl = e->lvl;
        auto& vs = adjLists[lvl][v];
        auto& us = adjLists[lvl][u];
        if (e->is_tree_edge()) {
            swap(vs, e->first_pos, threshold[lvl][v] - 1);
            swap(us, e->second_pos, threshold[lvl][u] - 1);
        }
        swap(vs, e->first_pos, vs.size() - 1);
        swap(us, e->second_pos, us.size() - 1);
        vs.pop_back();
        us.pop_back();
    }

    Edge::Edge(unsigned lvl, unsigned v, unsigned u) : lvl(lvl), v(v), u(u) {}

    void Edge::subscribe(unsigned first, unsigned second) {
        first_pos = first;
        second_pos = second;
    }

    unsigned Edge::level() {
        return lvl;
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

    void Edge::change_pos(unsigned vertex, unsigned pos) {
        if (u == vertex) {
            first_pos = pos;
        } else {
            second_pos = pos;
        }
    }

    unsigned Edge::opposite(unsigned w) {
        return v = w ? u : v;
    }

    EdgeToken::EdgeToken(Edge* edge) :edge(edge){}

    EdgeToken::EdgeToken(EdgeToken&& e) noexcept :edge(e.edge){
        e.edge = nullptr;
    }

    EdgeToken& EdgeToken::operator=(EdgeToken&& other) noexcept {
        edge = other.edge;
        other.edge = nullptr;
        return *this;
    }

    EdgeToken::EdgeToken() :edge(nullptr){}
}
