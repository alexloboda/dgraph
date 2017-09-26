#include "EulerTourForest.h"
#include <utility>

namespace dgraph {

    void Entry::splay() {
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

    void Entry::remove() {
        splay();
        if (left != nullptr) {
            left->parent = nullptr;
        }
        if (right != nullptr) {
            right->parent = nullptr;
        }
        if (left == nullptr){
            return;
        }
        if (right == nullptr){
            return;
        }
        merge(left, right);
        delete this;
    }

    void Entry::rotate(bool left_rotate){
        Entry* child = nullptr;
        if(left_rotate) {
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
            if (this == parent->left){
                parent->left = child;
            } else {
                parent->right = child;
            }
        }
        child->parent = parent;
        parent = child;
        recalc();
        child->recalc();
        if (parent != nullptr){
            parent->recalc();
        }
    }

    void merge(Entry* l, Entry* r) {
        if (l == nullptr || r == nullptr){
            return;
        }
        r = find_root(r);
        l = find_root(l)->rightmost();

        l->splay();
        l->right = r;
        r->parent = l;
        l->recalc();
    }

    Entry* find_root(Entry* e) {
        while (e->parent != nullptr) e = e->parent;
        return e;
    }

    Entry::Entry(int v, Entry* l, Entry* r, Entry* p) : left(l), right(r), parent(p), v(v), size(1) {}

    Entry* Entry::succ() {
        Entry* curr = this;
        if(right == nullptr){
            while (curr->parent != nullptr && curr == curr->parent->right) curr = curr->parent;
            if (curr->parent == nullptr){
                return nullptr;
            }
            return curr->parent;
        }
        curr = right;
        curr = curr->leftmost();
        return curr;
    }

    Entry* Entry::pred() {
        Entry* curr = this;
        if(left == nullptr){
            while (curr->parent != nullptr && curr == curr->parent->left) curr = curr->parent;
            if (curr->parent == nullptr){
                return nullptr;
            }
            return curr->parent;
        }
        curr = left;
        curr = curr->rightmost();
        return curr;
    }

    std::pair<Entry*, Entry*> split(Entry* e, bool keep_in_left) {
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

    void Entry::recalc() {
        size = 1;
        if(right != nullptr){
            size += right->size;
        }
        if(left != nullptr){
            size += left->size;
        }
    }

    EulerTourForest::EulerTourForest(int _n) : n(_n) {
        for (int i = 0; i < n; i++) {
            auto* vertex = new Entry(i);
            any.push_back(vertex);
        }
    }

    Entry* EulerTourForest::make_root(int v) {

    }


    std::pair<Entry*, Entry*> EulerTourForest::link(int v, int u) {
        Entry* root = make_root(v);
        make_root(u);
        auto* l = new Entry(v);
        auto* r = new Entry(u);
        merge(l, r);
        merge(root, l);
    }

    int EulerTourForest::cut(Entry* first, Entry* last) {

    }

    bool EulerTourForest::is_connected(int v, int u) {
        return find_root(first[v]) == find_root(first[u]);
    }

    void EulerTourForest::changeEdges(int v, int n) {
        first[v]->splay();
        first[v]->edges += n;
        first[v]->recalc();
    }

    int EulerTourForest::size(int v) {
        return find_root(first[v])->size;
    }

    Iterator EulerTourForest::iterator(int v){
        return first[v]->iterator();
    }

    std::string to_string(EulerTourForest& graph) {
        std::string str;
        std::vector<bool> vis(graph.n, false);
        for(int i = 0; i < graph.n; i++){
            Entry* curr = find_root(graph.first[i]);
            if(!vis[curr->vertex()]){
                vis[curr->vertex()] = true;
                str += to_string(curr) + "\n";
            }
        }
        return str;
    }

    Iterator::Iterator(Entry* entry) :entry(entry){}

    Iterator& Iterator::operator++() {
        if (entry->right != nullptr && entry->right->good()){
            entry = entry->right;
            while (true){
                if (entry->left != nullptr && entry->left->good()){
                    entry = entry->left;
                    continue;
                }
                if(entry->edges > 0){
                    return *this;
                }
                entry = entry->right;
            }
        }
        while (true) {
            if (entry->parent == nullptr) {
                entry = nullptr;
                return *this;
            }
            if (entry->parent->right != nullptr && entry->parent->right == entry){
                entry = entry->parent;
                continue;
            } else {
                break;
            }
        }
        if (entry->edges > 0){
            return *this;
        }
        return ++(*this);
    }

    int Iterator::operator*() {
        return entry->v;
    }

    bool Iterator::hasNext() {
        return entry != nullptr;
    }

    Iterator Entry::iterator() {
        Entry* curr = find_root(this)->leftmost();
        Iterator iterator(curr);
        if(!curr->good()) {
            ++iterator;
        }
        return iterator;
    }

    int Entry::vertex() {
        return v;
    }

    std::string to_string(Entry* e) {
        std::string str;
        e = e->leftmost();
        while(e != nullptr){
            str += std::to_string(e->v);
            e = e->succ();
        }
        return str;
    }

    bool Entry::good() {
        int sum = 0;
        if (left != nullptr){
            sum += left->edges;
        }
        if (right != nullptr){
            sum += right->edges;
        }
        return sum != edges;
    }

    Entry* Entry::leftmost() {
        Entry* curr = this;
        while (curr->left != nullptr) curr = curr->left;
        return curr;
    }

    Entry* Entry::rightmost() {
        Entry* curr = this;
        while (curr->right != nullptr) curr = curr->right;
        return curr;
    }

}