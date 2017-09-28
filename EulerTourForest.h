#ifndef DGRAPH_EULERTOURTREE_H
#define DGRAPH_EULERTOURTREE_H

#include <vector>
#include <string>

namespace dgraph {
    class Iterator;
    class EulerTourForest;

    class Entry {
        Entry* left;
        Entry* right;
        Entry* parent;
        unsigned v;
        unsigned size;
        unsigned edges;

        explicit Entry(unsigned , Entry* = nullptr, Entry* = nullptr, Entry* = nullptr);

        void splay();
        void rotate(bool);
        void remove();
        Entry* succ();
        Entry* leftmost();
        Entry* rightmost();
        void recalc();
        bool good();
        Iterator iterator();
        bool is_singleton();

        friend Entry* merge(Entry*, Entry*);
        friend std::pair<Entry*, Entry*> split(Entry*, bool);
        friend Entry* find_root(Entry* e);

        friend class EulerTourForest;
        friend class Iterator;

    public:
        unsigned vertex();

        friend std::string to_string(Entry*);
    };

    class Iterator {
        Entry* entry;
    public:
        explicit Iterator(Entry*);
        Iterator& operator++();
        unsigned operator*();
        bool hasNext();
    };

    class TreeEdge {
        Entry* edge;
        Entry* twin;
        TreeEdge(Entry*, Entry*);
    public:
        TreeEdge(TreeEdge&&) noexcept;
        ~TreeEdge() = default;

        friend class EulerTourForest;
    };

    class EulerTourForest {
        int n;
        std::vector<Entry*> any;
        Entry* make_root(unsigned v);
        Entry* expand(unsigned v);
        void change_any(Entry* e);
        void cutoff(Entry* e);
        void cut(Entry*, Entry*);

    public:
        explicit EulerTourForest(unsigned);
        EulerTourForest(const EulerTourForest&) = delete;
        EulerTourForest& operator=(const EulerTourForest&) = delete;
        EulerTourForest(EulerTourForest&&) noexcept;
        ~EulerTourForest();

        bool is_connected(unsigned v, unsigned u);
        TreeEdge link(unsigned v, unsigned u);
        void cut(TreeEdge&&);
        void changeEdges(unsigned v, int n);
        unsigned size(unsigned v);
        Iterator iterator(unsigned v);

        friend std::string to_string(EulerTourForest&);
    };
}

#endif //DGRAPH_EULERTOURTREE_H
