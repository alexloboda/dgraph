#ifndef DGRAPH_EDGE_H
#define DGRAPH_EDGE_H

#include "EulerTourForest.h"
#include "DynamicGraph.h"

namespace dgraph {

    class Edge {
        unsigned long lvl;
        std::vector<List*> links;
    public:
        explicit Edge(unsigned long);
        void subscribe(List*);
    };

}

#endif //DGRAPH_EDGE_H
