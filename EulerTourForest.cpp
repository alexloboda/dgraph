#include "EulerTourForest.h"

namespace dgraph {

    Entry* Entry::splay() {
        if (parent == nullptr) {
            return this;
        }
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

    void Entry::remove() {
        splay();
        left->parent = nullptr;
        right->parent = nullptr;
        merge(left, right);
    }

    void Entry::rotate(bool direction) {
        // TODO : speed up?
        if (direction) {
            left = left->right;
            left->parent = this;
            left->right = this;
            left->parent = parent;
            parent = left;
        } else {
            right = right->left;
            right->parent = this;
            right->left = this;
            right->parent = parent;
            parent = right;
        }
    }

    void merge(Entry* l, Entry* r) {
        Entry* e = l;
        e = findRoot(e);
        while (e->right != nullptr) e = e->right;
        e->splay();
        e->right = r;
        r->parent = e;
    }

    Entry* findRoot(Entry* e) {
        while (e->parent != nullptr) e = e->parent;
        return e;
    }

    Entry::Entry(Entry* l, Entry* r, Entry* p, int v) : left(l), right(r), parent(p), v(v) {}

    Entry* Entry::succ() {
        Entry* curr = this;
        if(right == nullptr){
            while (curr->parent != nullptr && curr->parent == curr->parent->parent->left) curr = curr->parent;
            if (curr->parent == nullptr){
                return nullptr;
            }
            curr = curr->parent;
            if(curr->right == nullptr){
                return nullptr;
            }
            curr = curr->right;
        } else {
            curr = right;
        }
        while(curr->left != nullptr) curr = curr->left;
        return curr;
    }

    Entry* Entry::pred() {
        Entry* curr = this;
        if(left == nullptr){
            while (curr->parent != nullptr && curr->parent == curr->parent->parent->right) curr = curr->parent;
            if (curr->parent == nullptr){
                return nullptr;
            }
            curr = curr->parent;
            if(curr->left == nullptr){
                return nullptr;
            }
            curr = curr->left;
        } else {
            curr = left;
        }
        while(curr->right != nullptr) curr = curr->right;
        return curr;
    }

    std::pair<Entry*, Entry*> dgraph::split(Entry* e, bool keep_in_left) {
        e->splay();
        Entry* left;
        Entry* right;
        if (keep_in_left) {
            left = e;
            right = e->right;
            e->right = nullptr;
            right->parent = nullptr;
        } else {
            left = e->left;
            right = e;
            e->left = nullptr;
            left->parent = nullptr;
        }
        return std::make_pair(left, right);
    }

    EulerTourForest::EulerTourForest(int _n) : n(_n) {
        for (int i = 0; i < n; i++) {
            auto* in = new Entry(nullptr, nullptr, nullptr, i);
            auto* out = new Entry(nullptr, nullptr, in, i);
            in->right = out;
            first.push_back(in);
            last.push_back(out);
        }
    }

    Entry* EulerTourForest::link(int v, int u) {

    }

    void EulerTourForest::cut(int v) {
        // TODO : delete one occurence of v's parent
        auto left_cut = split(first[v], false);
        auto right_cut = split(left_cut.second, true);
        merge(left_cut.first, right_cut.second);
    }

    bool EulerTourForest::is_connected(int v, int u) {
        return findRoot(first[v]) == findRoot(first[u]);
    }

}