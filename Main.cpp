#include <utility>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "DynamicGraph.h"
#include <random>
#include <tuple>
#include <assert.h>

#include <algorithm>

#include <gperftools/profiler.h>

namespace {
    class Tree;

    class Entry {
        Entry* parent;
        Entry* left;
        Entry* right;

        unsigned size;
        unsigned value;
        unsigned v;

        void splay() {
            while (parent != nullptr) {
                Entry* grandpa = parent->parent;
                bool is_left = parent->left == this;
                if (grandpa != nullptr) {
                    bool p_is_left = grandpa->left == parent;
                    if (is_left == p_is_left) {
                        grandpa->rotate(p_is_left);
                        parent->rotate(is_left);
                    } else {
                        parent->rotate(is_left);
                        grandpa->rotate(p_is_left);
                    }
                } else {
                    parent->rotate(is_left);
                }
            }
        }

        Entry* remove() {
            splay();
            if (left != nullptr) {
                left->parent = nullptr;
            }
            if (right != nullptr) {
                right->parent = nullptr;
            }
            if (left == nullptr) {
                return right;
            }
            if (right == nullptr) {
                return left;
            }
            return merge(left, right);
        }

        void rotate(bool left_rotate) {
            Entry* child = nullptr;
            if (left_rotate) {
                child = left;
                left = child->right;
                if (left != nullptr) {
                    left->parent = this;
                }
                child->right = this;
            } else {
                child = right;
                right = child->left;
                if (right != nullptr) {
                    right->parent = this;
                }
                child->left = this;
            }
            if (parent != nullptr) {
                if (this == parent->left) {
                    parent->left = child;
                } else {
                    parent->right = child;
                }
            }
            child->parent = parent;
            parent = child;
            recalc();
            child->recalc();
            if (parent != nullptr) {
                parent->recalc();
            }
        }

        friend Entry* merge(Entry* l, Entry* r) {
            if (l == nullptr) {
                return r;
            }
            if (r == nullptr) {
                return l;
            }
            r = find_root(r);
            l = find_root(l)->rightmost();

            l->splay();
            l->right = r;
            r->parent = l;
            l->recalc();
            return l;
        }

        friend Entry* find_root(Entry* e) {
            while (e->parent != nullptr) e = e->parent;
            return e;
        }

        Entry(unsigned v, unsigned value, Entry* l, Entry* r, Entry* p) : left(l), right(r), parent(p), v(v),
                                                                          size(1), value(value) {}

        Entry* find(int w) {
            if (v == w)
                return this;

            if (w < v) {
                if (left == nullptr) {
                    return this;
                } else {
                    return left->find(w);
                }
            } else {
                if (right == nullptr) {
                    return this;
                } else {
                    return right->find(w);
                }
            }
        }

        friend std::pair<Entry*, Entry*> split(Entry* e, bool keep_in_left) {
            e->splay();
            Entry* left;
            Entry* right;
            if (keep_in_left) {
                left = e;
                right = e->right;
                e->right = nullptr;
                left->recalc();
                if (right != nullptr) {
                    right->recalc();
                    right->parent = nullptr;
                }
            } else {
                left = e->left;
                right = e;
                e->left = nullptr;
                right->recalc();
                if (left != nullptr) {
                    left->recalc();
                    left->parent = nullptr;
                }
            }
            return std::make_pair(left, right);
        }

        void recalc() {
            size = 1;
            if (right != nullptr) {
                size += right->size;
            }
            if (left != nullptr) {
                size += left->size;
            }
        }

        Entry* rightmost() {
            Entry* curr = this;
            while (curr->right != nullptr) curr = curr->right;
            return curr;
        }

        friend class Tree;
    };

    class Tree {
        Entry* root;
    public:
        Tree() : root(nullptr) {}

        void add(unsigned v, unsigned val) {
            auto* new_entry = new Entry(v, val, nullptr, nullptr, nullptr);
            if (root == nullptr) {
                root = new_entry;
            } else {
                Entry* c = root->find(v);
                if (c->v == v) {
                    return;
                }
                std::pair<Entry*, Entry*> cut = split(c, c->v < v);
                root = merge(cut.first, new_entry);
                root = merge(root, cut.second);
            }
        }

        void remove(unsigned v) {
            auto e = root->find(v);
            root = e->remove();
        }

        unsigned size() {
            return root->size;
        }

        std::pair<unsigned, unsigned> kth(unsigned k) {
            Entry* curr = root;
            while (true) {
                if (curr->left != nullptr) {
                    if (curr->left->size > k) {
                        curr = curr->left;
                        continue;
                    } else {
                        k -= curr->left->size;
                    }
                }
                if (k == 0) {
                    return {curr->v, curr->value};
                } else {
                    k--;
                    curr = curr->right;
                }
            }
        }

        bool contains(unsigned v) {
            if (root == nullptr) {
                return false;
            }
            Entry* e = root->find(v);
            return e->v == v;
        }
    };

    using namespace std;
    using dgraph::DynamicGraph;
    using dgraph::EdgeToken;

    class Shuffler {
        long n;
        mt19937 mersenne;
        vector<Tree> edges;
        vector<EdgeToken> tokens;
        DynamicGraph graph;
        std::uniform_int_distribution<> rng_coin;
        vector<unsigned> cum;
    public:
        Shuffler(string filename, long permutations, long n) : graph(n), n(n) {
            ifstream fin(filename);
            random_device rd;
            mersenne = mt19937(43);
            vector<unsigned> degree(n, 0);
            edges.resize(n, Tree());
            unsigned num = 0;
            unsigned v, u;
            while (fin >> v >> u) {
                if (v == u) {
                    continue;
                }
                num++;
                EdgeToken token = graph.add(v, u);
                tokens.push_back(std::move(token));
                unsigned token_num = tokens.size() - 1;
                degree[v]++;
                degree[u]++;
                edges[v].add(u, token_num);
                edges[u].add(v, token_num);
            }
            cum.push_back(0);
            for (unsigned i = 0; i < n - 1; i++) {
                cum.push_back(cum[cum.size() - 1] + degree[i]);
            }
            unsigned number = cum[cum.size() - 1] + degree[degree.size() - 1];
            rng_coin = uniform_int_distribution<>(0, number - 1);
        }

        unsigned depth(){
            return graph.depth();
        }

        tuple<unsigned, unsigned, unsigned> pick_an_edge(){
            int k = rng_coin(mersenne);
            unsigned j = std::upper_bound(cum.begin(), cum.end(), k) - cum.begin() - 1;
            k -= cum[j];
            auto edge = edges[j].kth(k);
            return {j, edge.first, edge.second};
        }

        void shakeit(){
            while (true) {
                auto edge = pick_an_edge();
                auto other = pick_an_edge();
                unsigned v = get<0>(edge);
                unsigned u = get<1>(edge);
                unsigned w = get<0>(other);
                unsigned z = get<1>(other);
                unsigned e1 = get<2>(edge);
                unsigned e2 = get<2>(other);
                if (e1 == e2)
                    continue;
                if (v == z || w == u || edges[v].contains(z) || edges[w].contains(u)) {
                    continue;
                }
                graph.remove(std::move(tokens[e1]));
                graph.remove(std::move(tokens[e2]));
                tokens[e1] = std::move(graph.add(v, z));
                tokens[e2] = std::move(graph.add(w, u));
                if (graph.is_connected(v, u) && graph.is_connected(w, z)) {
                    edges[v].remove(u);
                    edges[u].remove(v);
                    edges[w].remove(z);
                    edges[z].remove(w);
                    edges[v].add(z, e1);
                    edges[z].add(v, e1);
                    edges[w].add(u, e2);
                    edges[u].add(w, e2);
                    return;
                }
                graph.remove(std::move(tokens[e1]));
                graph.remove(std::move(tokens[e2]));
                tokens[e1] = std::move(graph.add(v, u));
                tokens[e2] = std::move(graph.add(w, z));
            }
        }

        void print(){
            for (unsigned i = 0; i < n; i++) {
                for (unsigned j = 0; j < n; j++) {
                    std::cout << (edges[i].contains(j) ? "*" : ".") << " ";
                }
                std::cout << "\n";
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
