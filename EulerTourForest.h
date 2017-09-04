#ifndef DGRAPH_EULERTOURTREE_H
#define DGRAPH_EULERTOURTREE_H

#include <vector>
#include <iterator>

namespace dgraph {

class Entry {
    Entry* left;
    Entry* right;
    Entry* parent;
    int v;
    int size;
    int edges;
    bool good;
    Entry(Entry*, Entry*, Entry*, int);

    Entry* splay();
    void rotate(bool);
    void remove();
    Entry* succ();
    Entry* pred();
    void recalc();

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
    int cut(int v);
    void changeEdges(int v, int n);
};

class GoodIterator : public virtual std::iterator {
public:
    std::iterator operator++(int); //postfix increment
    int operator*() const;

    pointer operator->() const;

    friend bool operator==(const std::iterator &, const std::iterator &);

    friend bool operator!=(const std::iterator &, const std::iterator &);
};

#endif //DGRAPH_EULERTOURTREE_H
