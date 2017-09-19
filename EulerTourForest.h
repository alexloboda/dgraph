#ifndef DGRAPH_EULERTOURTREE_H
#define DGRAPH_EULERTOURTREE_H

#include <vector>
#include <string>

namespace dgraph {
    class Iterator;

    class Entry {
        Entry* left;
        Entry* right;
        Entry* parent;
        int v;
        int size;
        int edges;

        Entry(Entry*, Entry*, Entry*, int);
        void splay();
        void rotate(bool);
        void remove();
        Entry* succ();
        Entry* pred();
        Entry* leftmost();
        Entry* rightmost();
        void recalc();
        bool good();
        Iterator iterator();

        friend void merge(Entry*, Entry*);
        friend std::pair<Entry*, Entry*> split(Entry*, bool);
        friend Entry* find_root(Entry* e);

        friend class EulerTourForest;
        friend class Iterator;

    public:
        int vertex();

        friend std::string to_string(Entry*);
    };

    class Iterator {
        Entry* entry;
    public:
        explicit Iterator(Entry*);
        Iterator& operator++();
        int operator*();
        bool hasNext();
    };

    class EulerTourForest {
        int n;
        std::vector<Entry*> first;
        std::vector<Entry*> last;
        void make_root(int v);
        void check_order(int v);

    public:
        explicit EulerTourForest(int);
        bool is_connected(int v, int u);
        void link(int v, int u);
        int cut(int v, int u);
        void changeEdges(int v, int n);
        int size(int v);
        Iterator iterator(int v);

        friend std::string to_string(EulerTourForest&);
    };
}

#endif //DGRAPH_EULERTOURTREE_H
