#ifndef DGRAPH_DYNAMICGRAPH_H
#define DGRAPH_DYNAMICGRAPH_H

#include "EulerTourForest.h"

namespace dgraph {
    using std::vector;

    class List;

    class Edge {
        unsigned long lvl;
        std::vector<List*> links;
    public:
        explicit Edge(unsigned long);
        ~Edge();
        void subscribe(List*);
    };

    class DynamicGraph {
        int n;
        vector<EulerTourForest> forests;
        vector<vector<List*>> adjLists;
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
        List() = default;
        List* add(int, Edge*);
        ~List();
    };

}

#endif //DGRAPH_DYNAMICGRAPH_H
