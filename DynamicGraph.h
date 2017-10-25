#ifndef DGRAPH_DYNAMICGRAPH_H
#define DGRAPH_DYNAMICGRAPH_H

#include "EulerTourForest.h"

namespace {
    using std::vector;
}

namespace dgraph {
    class DynamicGraph;

    class Edge {
        unsigned lvl;
        unsigned v;
        unsigned u;
        unsigned first_pos;
        unsigned second_pos;
        std::vector<TreeEdge> tree_edges;
        void subscribe(unsigned, unsigned);
        void removeLinks();
        void add_tree_edge(TreeEdge&&);
    public:
        explicit Edge(unsigned, unsigned, unsigned);
        ~Edge() = default;

        void change_pos(unsigned, unsigned);
        unsigned from();
        unsigned to();
        unsigned level();
        bool is_tree_edge();

        friend class DynamicGraph;
    };

    class EdgeToken {
        Edge* edge;
        explicit EdgeToken(Edge*);
    public:
        EdgeToken();
        EdgeToken(const EdgeToken&) = delete;
        EdgeToken& operator=(const EdgeToken&) = delete;
        EdgeToken& operator=(EdgeToken&&) noexcept;
        EdgeToken(EdgeToken&&) noexcept;
        ~EdgeToken() = default;

        friend class DynamicGraph;
    };

    class DynamicGraph {
        unsigned n;
        unsigned size;
        vector<EulerTourForest> forests;
        vector<vector<vector<Edge*>>> adjLists;
        vector<vector<unsigned>> threshold;
        void downgrade(Edge* e);
    public:
        explicit DynamicGraph(unsigned n);
        DynamicGraph(const DynamicGraph&) = delete;
        DynamicGraph&operator=(const DynamicGraph&) = delete;
        ~DynamicGraph();

        EdgeToken add(unsigned v, unsigned u);
        void remove(EdgeToken&&);
        bool is_connected(unsigned v, unsigned u);
        bool is_connected();
        std::string str();
        unsigned degree(unsigned v);
    };
}

#endif //DGRAPH_DYNAMICGRAPH_H
