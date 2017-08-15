#ifndef DGRAPH_EULERTOURTREE_H
#define DGRAPH_EULERTOURTREE_H

#include <vector>

#include "Edge.h"

namespace dgraph {

class Entry {
    Entry* left;
    Entry* right;
    Entry* parent;
    int v;
    Entry(Entry*, Entry*, Entry, int);

    Entry* splay();
    friend void rotate(Entry*, bool);
    friend void merge(Entry*, Entry*);
    friend Entry* findRoot(Entry* e);
};

class EulerTourForest {
    int n;
    std::vector<Entry*> first;
    std::vector<Entry*> last;
public:
    EulerTourForest(int);

    bool is_connected(int v, int u);

    Entry* link(int v, int u);

    void cut(int v, int u);
};

}

#endif //DGRAPH_EULERTOURTREE_H
