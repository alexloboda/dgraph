package ctlab.dgraph;

import org.apache.commons.math3.util.Pair;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.NoSuchElementException;

class EulerTourForest {
    private int n;
    private List<Entry> any;
    private Entry anyRoot;

    EulerTourForest(int n) {
        this.n = n;
        anyRoot = null;
        any = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            any.add(new Entry(i));
        }
    }

    Entry makeRoot(int v) {
        Entry e = any.get(v);
        Pair<Entry, Entry> cut = Entry.split(e, false);
        return Entry.merge(cut.getSecond(), cut.getFirst());
    }

    Entry expand(int v) {
        Entry e = makeRoot(v);
        if (e.size == 1){
            return e;
        }
        Entry newNode = new Entry(v);
        Entry.merge(e, newNode);
        return newNode;
    }

    boolean isConnected(int v, int u) {
        if (isConnected()) {
            return true;
        }
        return Entry.findRoot(any.get(v)) == Entry.findRoot(any.get(u));
    }

    boolean isConnected() {
        return anyRoot != null && anyRoot.size == 2 * (n - 1);
    }

    void cutoff(Entry e, Entry replacement) {
        if (e.isSingletone()) {
            return;
        }
        if (any.get(e.v) == e){
            if (replacement == null) {
                changeAny(Entry.findRoot(e).leftmost());
            } else {
                changeAny(replacement);
            }
        }
        e.remove();
    }

    void changeEdges(int v, int n) {
        Entry curr = any.get(v);
        curr.edges = n;
        repairEdgesNumber(curr);
    }

    void incrementEdges(int v) {
        Entry curr = any.get(v);
        ++curr.edges;
        if (curr.edges == 1) {
            curr.good = true;
            repairEdgesNumber(curr.parent);
        }
    }

    void decrementEdges(int v) {
        Entry curr = any.get(v);
        --curr.edges;
        if (curr.edges == 0) {
            repairEdgesNumber(curr);
        }
    }

    int size(int v) {
        return Entry.findRoot(any.get(v)).size;
    }

    TreeEdge link(int v, int u) {
        Entry l = expand(v);
        Entry r = expand(u);
        anyRoot = Entry.merge(l, r);
        return new TreeEdge(l, r);
    }

    private void repairEdgesNumber(Entry curr) {
        while (curr != null) {
            boolean good = curr.edges > 0;
            if (curr.left != null) {
                good |= curr.left.good;
            }
            if (curr.right != null) {
                good |= curr.right.good;
            }
            if (good != curr.good) {
                curr.good = good;
                curr = curr.parent;
            } else {
                return;
            }
        }
    }

    private void changeAny(Entry e) {
        int edges = any.get(e.v).edges;
        int v = e.v;
        changeEdges(v, 0);
        any.set(v, e);
        changeEdges(v, edges);
    }

    void cut(TreeEdge edge) {
        if (edge.edge != null) {
            cut(edge.edge, edge.twin);
            edge.edge = null;
            edge.twin = null;
        }
    }

    private void cut(Entry first, Entry last) {
        anyRoot = null;
        Pair<Entry, Entry> firstCut = Entry.split(first, true);
        boolean right_ordered = firstCut.getSecond() != null &&
                Entry.findRoot(firstCut.getSecond()) == Entry.findRoot(last);
        Pair<Entry, Entry> secondCut = Entry.split(last, true);
        if (!right_ordered) {
            Pair<Entry, Entry> tmp = firstCut;
            firstCut = secondCut;
            secondCut = tmp;
        }

        Entry toRemove = firstCut.getFirst().rightmost();
        if (toRemove.isSingletone()) {
            if (secondCut.getSecond() != null) {
                changeAny(secondCut.getSecond().leftmost());
            }
        } else {
            Entry.merge(toRemove, secondCut.getSecond());
            Entry next = toRemove.succ();
            if (next == null) {
                cutoff(toRemove, null);
            } else {
                cutoff(toRemove, next);
            }
        }
        cutoff(secondCut.getFirst().rightmost(), null);
    }

    public Iterator<Integer> iterator(int v) {
        return any.get(v).iterator();
    }

    static class ETFIterator implements Iterator<Integer> {
        private Entry entry;

        ETFIterator(Entry entry) {
            this.entry = entry;
            if (!this.entry.good) {
                findNext();
            }
        }

        @Override
        public boolean hasNext() {
            return entry != null;
        }

        private void findNext() {
            while (true) {
                if (entry.right != null && entry.right.good) {
                    entry = entry.right;
                    while (true) {
                        if (entry.left != null && entry.left.good) {
                            entry = entry.left;
                            continue;
                        }
                        if (entry.edges > 0) {
                            return;
                        }
                        entry = entry.right;
                    }
                }
                while (true) {
                    if (entry.parent == null) {
                        entry = null;
                        return;
                    }
                    if (entry.parent.right != null && entry.parent.right == entry) {
                        entry = entry.parent;
                    } else {
                        entry = entry.parent;
                        break;
                    }
                }
                if (entry.edges > 0) {
                    return;
                }
            }
        }

        @Override
        public Integer next() {
            if (!hasNext()) {
                throw new NoSuchElementException();
            }
            int res = entry.v;
            findNext();
            return res;
        }
    }

    static class TreeEdge {
        Entry edge;
        Entry twin;

        TreeEdge(Entry edge, Entry twin) {
            this.edge = edge;
            this.twin = twin;
        }
    }

    static class Entry {
        private Entry left;
        private Entry right;
        private Entry parent;
        private int v;
        private int size;
        private int edges;
        private boolean good;

        Entry(int v) {
            this.v = v;
            this.size = 1;
            this.edges = 0;
            this.good = false;
        }

        void splay() {
            while (parent != null) {
                Entry grandpa = parent.parent;
                boolean isLeft = parent.left == this;
                if (grandpa != null) {
                    boolean pIsLeft = grandpa.left == parent;
                    if (isLeft == pIsLeft) {
                        grandpa.rotate(pIsLeft);
                        parent.rotate(isLeft);
                    } else {
                        parent.rotate(isLeft);
                        grandpa.rotate(pIsLeft);
                    }
                } else {
                    parent.rotate(isLeft);
                }
            }
        }

        void rotate(boolean leftRotate) {
            Entry child = null;
            if(leftRotate) {
                child = left;
                left = child.right;
                if (left != null) {
                    left.parent = this;
                }
                child.right = this;
            } else {
                child = right;
                right = child.left;
                if (right != null) {
                    right.parent = this;
                }
                child.left = this;
            }
            if (parent != null) {
                if (this == parent.left){
                    parent.left = child;
                } else {
                    parent.right = child;
                }
            }
            child.parent = parent;
            parent = child;
            recalc();
            child.recalc();
            if (parent != null){
                parent.recalc();
            }
        }

        public void remove() {
            splay();
            if (left != null) {
                left.parent = null;
            }
            if (right != null) {
                right.parent = null;
            }
            if (left == null || right == null) {
                return;
            }
            merge(left, right);
        }

        static Entry findRoot(Entry e) {
            while (e.parent != null) {
                e = e.parent;
            }
            return e;
        }

        static Entry merge(Entry l, Entry r) {
            if (l == null) {
                return r;
            }
            if (r == null) {
                return l;
            }
            r = findRoot(r);
            l = findRoot(l).rightmost();

            l.splay();
            l.right = r;
            r.parent = l;
            l.recalc();
            return l;
        }

        ETFIterator iterator() {
            Entry curr = findRoot(this).leftmost();
            return new ETFIterator(curr);
        }

        Entry succ() {
            Entry curr = this;
            if(right == null){
                while (curr.parent != null && curr == curr.parent.right) curr = curr.parent;
                if (curr.parent == null){
                    return null;
                }
                return curr.parent;
            }
            curr = right;
            curr = curr.leftmost();
            return curr;
        }

        static Pair<Entry, Entry> split(Entry e, boolean keepInLeft) {
            e.splay();
            Entry left;
            Entry right;
            if (keepInLeft) {
                left = e;
                right = e.right;
                e.right = null;
                left.recalc();
                if (right != null) {
                    right.recalc();
                    right.parent = null;
                }
            } else {
                left = e.left;
                right = e;
                e.left = null;
                right.recalc();
                if (left != null) {
                    left.recalc();
                    left.parent = null;
                }
            }
            return new Pair<>(left, right);
        }

        Entry leftmost() {
            Entry curr = this;
            while (curr.left != null) curr = curr.left;
            return curr;
        }

        Entry rightmost() {
            Entry curr = this;
            while (curr.right != null) curr = curr.right;
            return curr;
        }

        void recalc() {
            size = 1;
            good = edges > 0;
            if(right != null){
                size += right.size;
                good |= right.good;
            }
            if(left != null){
                size += left.size;
                good |= left.good;
            }
        }

        boolean isSingletone() {
            return parent == null && left == null && right == null;
        }
    }
}