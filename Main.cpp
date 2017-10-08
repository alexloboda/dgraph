#include <utility>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "DynamicGraph.h"
#include <random>
#include <tuple>
#include <unordered_set>

#include <algorithm>

#include <gperftools/profiler.h>

namespace {
    using namespace std;
    using dgraph::DynamicGraph;
    using dgraph::EdgeToken;

    class Shuffler {
        long n;
        vector<vector<bool>> edges;
        vector<EdgeToken> tokens;
        vector<pair<unsigned , unsigned>> ends;
        DynamicGraph graph;
        std::uniform_int_distribution<unsigned> rng_coin;
        std::uniform_int_distribution<unsigned> rng_for_ends;
        mt19937 mersenne;
    public:
        Shuffler(string filename, long permutations, long n) : graph(n), n(n) {
            ifstream fin(filename);
            random_device rd;
            mersenne = mt19937(43);
            edges.resize(n, vector<bool>(n, false));
            unsigned v, u;
            while (fin >> v >> u) {
                if (v == u) {
                    continue;
                }
                EdgeToken token = graph.add(v, u);
                tokens.push_back(std::move(token));
                edges[v][u] = true;
                edges[u][v] = true;
                ends.emplace_back(v, u);
            }
            rng_coin = uniform_int_distribution<unsigned>(0, tokens.size() - 1);
            rng_for_ends = uniform_int_distribution<unsigned>(0, 3);
        }

        unsigned depth(){
            return graph.depth();
        }

        void shakeit(){
            while (true) {
                unsigned e1 = rng_coin(mersenne);
                unsigned e2 = rng_coin(mersenne);
                unsigned ends_direct = rng_for_ends(mersenne);
                auto edge = ends[e1];
                auto other = ends[e2];
                unsigned v = edge.first;
                unsigned u = edge.second;
                unsigned w = other.first;
                unsigned z = other.second;

                if (ends_direct / 2 == 0) {
                    std::swap(v, u);
                }

                if (ends_direct % 2 == 0) {
                    std::swap(w, z);
                }

                if (e1 == e2)
                    continue;
                if (v == z || w == u || edges[v][z] || edges[w][u]) {
                    continue;
                }
                graph.remove(std::move(tokens[e1]));
                graph.remove(std::move(tokens[e2]));
                tokens[e1] = std::move(graph.add(v, z));
                tokens[e2] = std::move(graph.add(w, u));
                if (graph.is_connected(v, u) && graph.is_connected(w, z)) {
                    edges[v][u] = false;
                    edges[u][v] = false;
                    edges[w][z] = false;
                    edges[z][w] = false;
                    edges[v][z] = true;
                    edges[z][v] = true;
                    edges[w][u] = true;
                    edges[u][w] = true;
                    ends[e1] = make_pair(v, z);
                    ends[e2] = make_pair(w, u);
                    return;
                }
                graph.remove(std::move(tokens[e1]));
                graph.remove(std::move(tokens[e2]));
                tokens[e1] = std::move(graph.add(v, u));
                tokens[e2] = std::move(graph.add(w, z));
            }
        }
    };
}


int main(int argc, char** argv) {
    ProfilerStart("../a.prof");
    istringstream pss(argv[1]);
    unsigned permutations;
    pss >> permutations;
    istringstream nss(argv[2]);
    unsigned n;
    nss >> n;
    Shuffler s("../net", permutations, n);
    for (int i = 0; i < permutations; i++) {
        s.shakeit();
    }
    std::cout << s.depth() / (double)n << std::endl;
    ProfilerStop();
}
