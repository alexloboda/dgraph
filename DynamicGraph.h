#ifndef DGRAPH_DYNAMICGRAPH_H
#define DGRAPH_DYNAMICGRAPH_H

#include "EulerTourForest.h"

namespace dgraph {
    using std::vector;

    class List;

    class Edge{
        unsigned lvl;
        std::vector<List*> links;
    public:
        unsigned level();
        int where();
        explicit Edge(unsigned);
        ~Edge();
        void subscribe(List*);
    };

    class DynamicGraph {
        int n;
        int size;
        vector<EulerTourForest> forests;
        vector<vector<List*>> adjLists;
        vector<int> parent;
    public:
        explicit DynamicGraph(int n);
        Edge* add(int v, int u);
        void remove(Edge*);
        bool is_connected(int v, int u);
    };

    class List {
        Edge* edge;
        int u;
        List* next;
        List* prev;
        List(int, Edge*, List*, List*);
    public:
        List();
        List* add(int, Edge*);
        ~List();

        friend class Edge;
    };
}

#endif //DGRAPH_DYNAMICGRAPH_H
