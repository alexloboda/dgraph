#ifndef DGRAPH_DYNAMICGRAPH_H
#define DGRAPH_DYNAMICGRAPH_H

#include "EulerTourForest.h"

namespace dgraph {
    using std::vector;

    class List;
    class ListIterator;

    class Edge {
        unsigned lvl;
        unsigned v;
        unsigned u;
        std::vector<List*> links;
        std::vector<TreeEdge> tree_edges;
        void subscribe(List*);
        void removeLinks();
        void add_tree_edge(TreeEdge&&);
    public:
        explicit Edge(unsigned, unsigned , unsigned);
        ~Edge();

        unsigned from();
        unsigned to();
        unsigned level();
        bool is_tree_edge();

        friend class DynamicGraph;
    };

    class DynamicGraph {
        unsigned n;
        unsigned size;
        vector<EulerTourForest> forests;
        vector<vector<List*>> adjLists;
        void downgrade(Edge* e);
    public:
        explicit DynamicGraph(unsigned n);
        Edge* add(unsigned v, unsigned u);
        void remove(Edge*);
        bool is_connected(unsigned v, unsigned u);

        friend std::string to_string(DynamicGraph&);
    };

    class List {
        Edge* edge;
        unsigned u;
        List* next;
        List* prev;
        List(unsigned, Edge*, List*, List*);
    public:
        List();

        List* add(unsigned , Edge*);
        ListIterator iterator();
        unsigned vertex();
        Edge* e();
        ~List();

        friend class Edge;
        friend class ListIterator;
    };

    class ListIterator {
        List* list;
    public:
        explicit ListIterator(List*);
        ListIterator operator++(int);
        List* operator*();
        bool hasNext();
    };
}

#endif //DGRAPH_DYNAMICGRAPH_H
