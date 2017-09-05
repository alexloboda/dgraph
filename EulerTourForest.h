#ifndef DGRAPH_EULERTOURTREE_H
#define DGRAPH_EULERTOURTREE_H

#include <vector>

namespace dgraph {
    class Iterator;

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
        friend class Iterator;

        Iterator begin();
        Iterator end();
    };

    class Iterator {
        Entry* entry;
    public:
        explicit Iterator(Entry*);
        Iterator operator++(int);
        int operator*();
        friend bool operator==(const Iterator&, const Iterator&);
        friend bool operator!=(const Iterator&, const Iterator&);
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
}

#endif //DGRAPH_EULERTOURTREE_H
