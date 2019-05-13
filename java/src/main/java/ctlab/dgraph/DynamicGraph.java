package ctlab.dgraph;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * This class implements an algorithm for fully dynamic connectivity. It allows to add edges, remove edges and
 * check connectivity between arbitrary vertices as well as check that overall graph is connected. The only limitation
 * is that number of vertices must be fixed at the moment of object construction.
 *
 * @author Alexander Loboda
 */
public class DynamicGraph {
    private int n;
    private int size;
    private List<EulerTourForest> forests;
    private List<List<LinkedList>> adjLists;

    /**
     * Constructs a new dynamic graph with n vertices. Vertices are numbered from 0 to n - 1.
     * @param n Number of vertices.
     */
    public DynamicGraph(int n) {
        if (n <= 0) {
            throw new IllegalArgumentException();
        }
        this.n = n;
        size = (int)Math.round(Math.ceil(Math.log(n) / Math.log(2)) + 1);
        forests = new ArrayList<>();
        adjLists = new ArrayList<>();
        for (int i = 0; i < size; i++) {
            forests.add(new EulerTourForest(n));
            adjLists.add(new ArrayList<>());
            for (int j = 0; j < n; j++) {
                adjLists.get(i).add(new LinkedList());
            }
        }
    }

    /**
     * Returns true if a path connecting vertices v and u exists.
     * @param v a vertex
     * @param u a vertex
     * @return boolean
     */
    public boolean isConnected(int v, int u) {
        return forests.get(forests.size() - 1).isConnected(v, u);
    }

    /**
     * Returns true if all n vertices are connected.
     * @return boolean
     */
    public boolean isConnected() {
        return forests.get(forests.size() - 1).isConnected();
    }

    /**
     * Adds an edge between vertices v and u to the graph.
     * @param v a vertex
     * @param u a vertex
     * @return a token that could be used to remove the edge from the graph
     */
    public EdgeToken add(int v, int u) {
        if (v == u) {
            return new EdgeToken(null);
        }
        if (v >= this.n || u >= this.n || v < 0 || u < 0) {
            throw new IllegalArgumentException("No such vertex in the graph");
        }
        int n = size - 1;
        Edge edge = new Edge(n, v, u);
        if (!isConnected(v, u)) {
            edge.addTreeEdge(forests.get(n).link(v, u));
        }
        forests.get(n).incrementEdges(v);
        forests.get(n).incrementEdges(u);
        edge.subscribe(adjLists.get(n).get(v).add(u, edge),
                       adjLists.get(n).get(u).add(v, edge));
        return new EdgeToken(edge);
    }

    /**
     * Removes previously added edge from the graph. The operation requires a token of corresponding addition operation.
     * Applying this method twice on the same token has no effect.
     * @param token a token
     */
    public void remove(EdgeToken token) {
        Edge link = token.edge;
        token.edge = null;
        if (link == null) {
            return;
        }

        int v = link.from();
        int u = link.to();
        boolean complex_deletion = link.isTreeEdge();
        int level = link.level();

        if (complex_deletion) {
            for (int i = 0; i <= size - level - 1; i++){
                forests.get(size - i - 1).cut(link.treeEdges.get(i));
            }
        }

        forests.get(level).decrementEdges(v);
        forests.get(level).decrementEdges(u);

        link.removeLinks();

        if (complex_deletion) {
            for (int i = level; i < size; i++){
                // find new connection
                // to do that choose the lesser component
                if(forests.get(i).size(v) > forests.get(i).size(u)){
                    int t = v;
                    v = u;
                    u = t;
                }
                // and iterate over good vertices until success
                // propagating all tree edges of smallest component
                Edge replacement = null;
                Iterator<Integer> it = forests.get(i).iterator(v);
                while(it.hasNext()){
                    Integer w = it.next();
                    Iterator<LinkedList> lit = adjLists.get(i).get(w).iterator();
                    while(lit.hasNext()){
                        LinkedList l = lit.next();
                        Edge e = l.edge;
                        int up = l.vertex();
                        if (e.isTreeEdge()) {
                            downgrade(e);
                        } else {
                            if (replacement != null) {
                                break;
                            }
                            if (isConnected(up, u)) {
                                replacement = e;
                            } else {
                                downgrade(e);
                            }
                        }
                    }
                }

                if (replacement != null) {
                    for (int j = size - 1; j >= i; j--){
                        replacement.addTreeEdge(forests.get(j).link(replacement.v, replacement.u));
                    }
                    break;
                }
            }
        }
    }

    void downgrade(Edge e){
        int v = e.from();
        int w = e.to();
        int lvl = e.lvl--;
        e.removeLinks();
        e.subscribe(adjLists.get(lvl - 1).get(w).add(v, e), adjLists.get(lvl - 1).get(v).add(w, e));
        forests.get(lvl).decrementEdges(w);
        forests.get(lvl).decrementEdges(v);
        forests.get(lvl - 1).incrementEdges(w);
        forests.get(lvl - 1).incrementEdges(v);
        if (e.isTreeEdge()) {
            e.addTreeEdge(forests.get(lvl - 1).link(v, w));
        }
    }

    public static class EdgeToken {
        private Edge edge;

        EdgeToken(Edge edge) {
            this.edge = edge;
        }
    }

    static class Edge {
        private int lvl;
        private int v;
        private int u;
        private LinkedList firstLink;
        private LinkedList secondLink;
        private List<EulerTourForest.TreeEdge> treeEdges;

        Edge(int lvl, int v, int u) {
            this.lvl = lvl;
            this.v = v;
            this.u = u;
            treeEdges = new ArrayList<>();
        }

        void subscribe(LinkedList first, LinkedList second) {
            firstLink = first;
            secondLink = second;
        }

        int level() {
            return lvl;
        }

        void removeLinks() {
            firstLink.remove();
            secondLink.remove();
            firstLink = null;
            secondLink = null;
        }

        int from() {
            return v;
        }

        int to() {
            return u;
        }

        void addTreeEdge(EulerTourForest.TreeEdge edge) {
            treeEdges.add(edge);
        }

        boolean isTreeEdge() {
            return !treeEdges.isEmpty();
        }
    }

    static class LinkedList {
        private Edge edge;
        private int u;
        private LinkedList next;
        private LinkedList prev;

        LinkedList(int u, Edge edge, LinkedList prev, LinkedList next) {
            this.u = u;
            this.edge = edge;
            this.next = next;
            this.prev = prev;
        }

        LinkedList add(int v, Edge edge) {
            LinkedList newList = new LinkedList(v, edge, prev, this);
            prev.next = newList;
            prev = newList;
            return newList;
        }

        void remove() {
            next.prev = prev;
            prev.next = next;
        }

        LinkedList() {
            next = this;
            prev = this;
            u = Integer.MAX_VALUE;
        }

        Iterator<LinkedList> iterator() {
            return new ListIterator(next);
        }

        int vertex() {
            return u;
        }

        Edge e() {
            return edge;
        }
    }

    static class ListIterator implements Iterator<LinkedList> {
        private LinkedList list;

        ListIterator(LinkedList list) {
            this.list = list;
        }

        @Override
        public boolean hasNext() {
            return list.edge != null;
        }

        @Override
        public LinkedList next() {
            LinkedList ret = list;
            list = list.next;
            return ret;
        }
    }
}
