#ifndef DGRAPH_DYNAMICGRAPH_H
#define DGRAPH_DYNAMICGRAPH_H

#include "EulerTourForest.h"

namespace dgraph {
    using std::vector;

    class List;
    class ListIterator;

    class Edge{
        unsigned lvl;
        std::vector<List*> links;
    public:
        unsigned level();
        int where();
        explicit Edge(unsigned);
        ~Edge();
        void levelDown();
        void subscribe(List*);
        void removeLinks();
    };

    class DynamicGraph {
        int n;
        int size;
        vector<EulerTourForest> forests;
        vector<vector<List*>> adjLists;
        vector<int> parent;
        void downgrade(int v, int w, Edge* e);
    public:
        explicit DynamicGraph(int n);
        Edge* add(int v, int u);
        void remove(Edge*);
        bool is_connected(int v, int u);

        friend std::string to_string(DynamicGraph&);
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
        ListIterator iterator();
        int vertex();
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
