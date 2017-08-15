#include "EulerTourForest.h"

using namespace dgraph;

Entry* Entry::splay() {
    if(parent == nullptr){
        return this;
    }
    Entry* grandpa = parent->parent;
    bool is_left = parent->left == this;
    if(grandpa != nullptr){
        bool p_is_left = grandpa->left == parent;
        if(is_left == p_is_left){
            rotate(grandpa, p_is_left);
            rotate(parent, is_left);
        } else {
            rotate(parent, is_left);
            rotate(grandpa, p_is_left);
        }
    } else {
        rotate(parent, is_lelf);
    }
}

void Entry::rotate(Entry* e, bool left){
    // TODO : speed up?
    Entry* child = nullptr;
    if(left){
        child = e->left;
        e->left = child->right;
        e->left->parent = e;
        child->right = e;
        child->parent = e->parent;
        e->parent = child;
    } else {
        child = e->right;
        e->right = child->left;
        e->right->parent = e;
        child->left = e;
        child->parent = e->parent;
        e->parent = child;
    }
}

void Entry::merge(Entry* l, Entry* r) {
    Entry* e = l;
    e = findRoot(e);
    while(e->right != nullptr) e = e->right;
    e->splay();
    e->right = r;
    r->parent = e;
}

Entry* Entry::findRoot(Entry *e) {
    while(e->parent != nullptr) e = e->parent;
    return e;
}

Entry::Entry(Entry* l, Entry* r, Entry p, int v) :left(l), right(r), parent(p) {}

EulerTourForest::EulerTourTree(int _n) :n(_n){
    for(int i = 0; i < n; i++){
        Entry* in = new Entry(nullptr, nullptr, nullptr, i);
        Entry* out = new Entry(nullptr, nullptr, in, i);
        in->right = out;
        first.push_back(in);
        last.push_back(out);
    }
}

Edge* EulerTourForest::link(int v, int u) {

}

void EulerTourForest::cut(int v, int u) {
    
}

bool EulerTourForest::is_connected(int v, int u) {
    return findRoot(first[v]) == findRoot(first[u]);
}
