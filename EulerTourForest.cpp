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
                //TODO: Bug here?
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
        // TODO : speed up?
        Entry* child = nullptr;
        if(left_rotate) {
            child = left;
            left = child->right;
            if (left != nullptr) {
                left->parent = this;
            }
            child->right = this;
            child->parent = parent;
            parent = child;
        } else {
            child = right;
            right = child->left;
            if (right != nullptr) {
                right->parent = this;
            }
            child->left = this;
            child->parent = parent;
            parent = child;
        }
        recalc();
        child->recalc();
    }

    void merge(Entry* l, Entry* r) {
        if (l == nullptr || r == nullptr){
            return;
        }
        r = findRoot(r);
        l = findRoot(l);
        while (l->right != nullptr) l = l->right;
        l->splay();
        l->right = r;
        r->parent = l;
        l->recalc();
    }

    Entry* findRoot(Entry* e) {
        while (e->parent != nullptr) e = e->parent;
        return e;
    }

    Entry::Entry(Entry* l, Entry* r, Entry* p, int v) : left(l), right(r), parent(p), v(v) {}

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
        while(curr->left != nullptr) curr = curr->left;
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
        while (curr->right != nullptr) curr = curr->right;
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
        good = edges > 0;
        if(right != nullptr){
            size += right->size;
            good |= right->good;
        }
        if(left != nullptr){
            size += left->size;
            good |= left->good;
        }
    }

    EulerTourForest::EulerTourForest(int _n) : n(_n) {
        for (int i = 0; i < n; i++) {
            auto* vertex = new Entry(nullptr, nullptr, nullptr, i);
            vertex->recalc();
            first.push_back(vertex);
            last.push_back(vertex);
        }
    }

    void EulerTourForest::link(int v, int u) {
        auto cut = split(last[v], false);
        auto* node = new Entry(nullptr, nullptr, nullptr, v);
        node->recalc();
        if(first[v] == last[v]){
            first[v] = node;
        }
        merge(node, first[u]);
        merge(cut.first, node);
        merge(node, cut.second);
    }

    int EulerTourForest::cut(int v) {
        Entry* prev = first[v]->pred();
        if(prev != first[prev->v]){
            prev->remove();
        } else {
            Entry* next = last[v]->succ();
            if(next == last[next->v]){
                last[next->v] = prev;
            }
            next->remove();
        }
        auto left_cut = split(first[v], false);
        auto right_cut = split(left_cut.second, true);
        merge(left_cut.first, right_cut.second);
    }

    bool EulerTourForest::is_connected(int v, int u) {
        return findRoot(first[v]) == findRoot(first[u]);
    }

    void EulerTourForest::changeEdges(int v, int n) {
        first[v]->splay();
        first[v]->edges += n;
        first[v]->recalc();
    }

    int EulerTourForest::size(int v) {
        return findRoot(first[v])->size;
    }

    Iterator EulerTourForest::iterator(int v){
        return first[v]->iterator();
    }

    Iterator::Iterator(Entry* entry) :entry(entry){}

    Iterator Iterator::operator++(int) {
        if (entry->right != nullptr && entry->right->good){
            entry = entry->right;
            while (true){
                if (entry->left != nullptr && entry->left->good){
                    entry = entry->left;
                    continue;
                }
                if(entry->edges > 0){
                    return *this;
                }
                entry = entry->right;
            }
        }
        if (entry->parent == nullptr){
            entry = nullptr;
            return *this;
        }
        entry = entry->parent;
        if (entry->edges > 0){
            return *this;
        }
        return (*this)++;
    }

    int Iterator::operator*() {
        return entry->v;
    }

    bool Iterator::hasNext() {
        return entry != nullptr;
    }

    Iterator Entry::iterator() {
        Entry* curr = findRoot(this);
        while (curr->left != nullptr) curr = curr->left;
        Iterator iterator(curr);
        iterator++;
        return iterator;
    }
}