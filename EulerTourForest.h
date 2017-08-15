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
    Entry(Entry*, Entry*, Entry*, int);

    Entry* splay();
    void rotate(bool);
    void remove();
    Entry* succ();
    Entry* pred();

    friend void merge(Entry*, Entry*);
    friend std::pair<Entry*, Entry*> split(Entry*, bool);
    friend Entry* findRoot(Entry* e);

    friend class EulerTourForest;
};

class EulerTourForest {
    int n;
    std::vector<Entry*> first;
    std::vector<Entry*> last;
public:
    explicit EulerTourForest(int);

    bool is_connected(int v, int u);
    void link(int v, int u);
    void cut(int v);
};

}

#endif //DGRAPH_EULERTOURTREE_H
