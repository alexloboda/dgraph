#ifndef DGRAPH_DYNAMICGRAPH_H
#define DGRAPH_DYNAMICGRAPH_H

#include "EulerTourForest.h"

namespace dgraph {

    class DynamicGraph {
        int n;
        std::vector<EulerTourForest> forests;
    public:
        explicit DynamicGraph(int n);
        Edge* add(int v, int u);
        void remove(Edge*);
        bool is_connected(int v, int u);
    };

    class List {

    };

    class AdjList {
        std::vector<List> adj;
    public:
        explicit AdjList(int);
    };

}

#endif //DGRAPH_DYNAMICGRAPH_H
