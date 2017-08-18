#include "Edge.h"

namespace dgraph {

    Edge::Edge(unsigned long lvl) : lvl(lvl) {}

    void Edge::subscribe(List* link) {
        links.push_back(link);
    }
}

