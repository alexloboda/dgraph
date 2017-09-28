#include "catch.hpp"

#include "../DynamicGraph.h"
#include <queue>
#include <random>
#include <map>

namespace {
    using std::vector;
    using std::queue;

    class ReferenceGraph {
        vector<vector<bool>> adj;
    public:
        explicit ReferenceGraph(unsigned n){
            adj.resize(n, vector<bool>(n, false));
        }

        void add(unsigned v, unsigned u) {
            adj[v][u] = true;
            adj[u][v] = true;
        }

        void remove(unsigned v, unsigned u) {
            adj[v][u] = false;
            adj[u][v] = false;
        }

        bool is_edge(unsigned v, unsigned u){
            return adj[v][u];
        }

        bool is_connected(unsigned v, unsigned u){
            vector<bool> vis(adj.size(), false);
            queue<unsigned> q;
            vis[v] = true;
            q.push(v);
            while (!q.empty()) {
                unsigned w = q.front();
                q.pop();
                for (unsigned i = 0; i < adj.size(); i++) {
                    if (adj[w][i] && !vis[i]) {
                        vis[i] = true;
                        q.push(i);
                    }
                }
            }
            return vis[u];
        }
    };

    void check(unsigned size, dgraph::DynamicGraph& graph, ReferenceGraph& reference){
        for (unsigned i = 0; i < size; i++) {
            for (unsigned j = 0; j < size; j++) {
                bool expected = reference.is_connected(i, j);
                INFO(i << " and " << j << " actually are " << (expected ? "" : " not") << "connected");
                REQUIRE(graph.is_connected(i, j) == expected);
            }
        }
    }
}

TEST_CASE("dynamic graphs work fine on simple tests", "[dg]"){
    SECTION("simple triangle test") {
        dgraph::DynamicGraph graph(3);
        REQUIRE(!graph.is_connected(0, 1));
        REQUIRE(!graph.is_connected(1, 2));
        REQUIRE(!graph.is_connected(0, 2));
        auto token = graph.add(0, 1);
        REQUIRE(graph.is_connected(0, 1));
        REQUIRE(!graph.is_connected(0, 2));
        graph.add(1, 2);
        graph.add(0, 2);
        REQUIRE(graph.is_connected(0, 1));
        REQUIRE(graph.is_connected(1, 2));
        REQUIRE(graph.is_connected(0, 2));
        graph.remove(std::move(token));
        REQUIRE(graph.is_connected(0, 1));
        REQUIRE(graph.is_connected(1, 2));
        REQUIRE(graph.is_connected(0, 2));
    }

    SECTION("random operations on small graph"){
        const unsigned size = 5;
        const unsigned ops = 100;
        std::srand(42);

        ReferenceGraph reference(size);
        dgraph::DynamicGraph graph(size);
        std::map<int, std::map<int, dgraph::EdgeToken>> tokens;
        for (unsigned i = 0; i < size; i++) {
            tokens[i] = std::map<int, dgraph::EdgeToken>();
        }
        check(size, graph, reference);
        for (unsigned i = 0; i < ops; i++) {
            auto v = rand() % size;
            auto u = rand() % size;
            if (reference.is_edge(v, u)) {
                reference.remove(v, u);
                if (!tokens[v].count(u)) {
                    std::swap(v, u);
                }
                graph.remove(std::move(tokens[v][u]));
                tokens[v].erase(u);
            } else {
                reference.add(v, u);
                tokens[v].insert(std::make_pair(u, std::move(graph.add(v, u))));
            }
            check(size, graph, reference);
        }
    }
}
