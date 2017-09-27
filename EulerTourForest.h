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

        Entry(int, Entry* = nullptr, Entry* = nullptr, Entry* = nullptr);
        void splay();
        void rotate(bool);
        Entry* remove();
        Entry* succ();
        Entry* leftmost();
        Entry* rightmost();
        void recalc();
        bool good();
        Iterator iterator();

        friend Entry* merge(Entry*, Entry*);
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
        std::vector<Entry*> any;
        Entry* make_root(int v);
        Entry* expand(int v);
        void change_any(Entry* e);
        Entry* cutoff(Entry* e);

    public:
        explicit EulerTourForest(int);
        bool is_connected(int v, int u);
        std::pair<Entry*, Entry*> link(int v, int u);
        void cut(Entry*, Entry*);
        void changeEdges(int v, int n);
        int size(int v);
        Iterator iterator(int v);

        friend std::string to_string(EulerTourForest&);
    };
}

#endif //DGRAPH_EULERTOURTREE_H
