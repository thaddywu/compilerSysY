#include <bits/stdc++.h>
using namespace std;

class dataCell {
public:
    int depth;
    dataCell(int _depth): depth(_depth) {}
    virtual void debug() { assert(false); }
    virtual void instantialize() { assert(false); }
    virtual void initialize(string &eeyore, vector<int> &dim, int addr, bool var) { assert(false); }
};
class dataAggr: public dataCell{
public:
    vector<dataCell*> aggr;
    dataAggr(int _depth): dataCell(_depth) {}
    void condense(vector<int> &dim) {
        /* merge cells in the end of the same depth. */
        assert(!aggr.empty());
        int depth = aggr.back()->depth;
        assert(depth > 0);
        dataAggr *cds = new dataAggr(depth - 1);
        for (int k = 0; k < dim[depth - 1]; k++)
        {
            if (aggr.empty()) continue;
            dataCell* back = aggr.back();
            if (back->depth != depth) continue;
            cds->aggr.push_back(back);
            aggr.pop_back();
        }
        reverse(cds->aggr.begin(), cds->aggr.end());
        aggr.push_back(cds);
    }
    void merge(vector<int> &dim) {
        /* merge only when #cells reaches the array's boundray. */
        while (!aggr.empty()) {
            int depth = aggr.back()->depth;
            if (depth == 0) {
                assert(aggr.size() == 1);
                return ;
            }
            int sz = aggr.size(), n = dim[depth - 1];
            if (sz < n || aggr[sz-n]->depth != depth) return ;
            condense(dim);
        }
    }
    void liftup(vector<int> &dim, int depth) {
        /* lift node to given depth, every time condense the end,
            but remember to merge the end immediately !! */
        while (!aggr.empty() && aggr.back()->depth > depth)
            { condense(dim); merge(dim); }
    }
    dataAggr* rework(vector<int> &dim) {
        /* generate the final node after processing with all siblings, 
            and return its pointer */
        assert(!aggr.empty());
        while (aggr[0]->depth != aggr.back()->depth)
            condense(dim);
        depth = aggr[0]->depth - 1;
        return this;
    }
    virtual void debug() {
        printf("{ ");
        bool colon = false;
        for (auto x: aggr) {
            if (colon) printf(", ");
            x->debug();
            colon = true;
        }
        printf(" }");
    }
    virtual void instantialize() {
        for (auto child: aggr)
            child->instantialize();
    }
    virtual void initialize(string &eeyore, vector<int> &dim, int addr, bool var) {
        addr *= dim[depth];
        for (int i = 0; i < aggr.size(); i++)
            aggr[i]->initialize(eeyore, dim, addr + i, var);
    }
};
class dataLeaf: public dataCell{
public:
    nodeAST *expr;
    dataLeaf(int _depth, nodeAST *_expr): dataCell(_depth), expr(_expr) {}
    virtual void debug() { if (expr) printf("%d", expr->eval() ); else printf("x"); }
    virtual void instantialize() { if (expr) expr = new _INTEGER(expr->eval()); }
    virtual void initialize(string &eeyore, vector<int> &dim, int addr, bool var) {
        if (expr == NULL) return ;
        string t = expr->atomize();
        if (var)
            printf("\t%s = %s\n", eeyore.c_str(), t.c_str());
        else
            printf("\t%s [ %d ] = %s\n", eeyore.c_str(), addr * 4, t.c_str());
    }
};

class dataDescript {
public:
    string token;
    string eeyore; /* converted token in eeyore */
    vector<int> dim;
    dataCell *inits;
    dataDescript(string _token, vector<int> _dim, _TREE *_node):
        token(_token), dim(_dim), inits(NULL) { 
        if (!_node) return ;
        inits = ((dataAggr *)construct(new dataAggr(-1), _node))->aggr[0];
        while (inits->depth > 0) {
            dataAggr *anc = new dataAggr(inits->depth - 1);
            anc->aggr.push_back(inits);
            inits = anc;
        }
    }

    int eval(vector<int> &addr) {
        if (inits == NULL) return 0;
        dataAggr *node = (dataAggr *)inits;
        for (auto x: addr) {
            if (node->aggr.size() < x) return 0;
            if (!node->aggr[x]) return 0;
            node = (dataAggr *)node->aggr[x];
        }
        return ((dataLeaf *)node)->expr->eval();
    }
    int eval() {return inits ? ((dataLeaf *)inits)->expr->eval() : 0;}
private:
    dataCell* construct(dataAggr *tree, _TREE *node) {
        dataCell *incr = NULL;
        if (node->leaf())
            incr = (dataCell*) new dataLeaf(dim.size(), node->getExpr());
        else
            incr = construct(new dataAggr(-1), node->getChild());
        tree->liftup(dim, incr->depth);
        tree->aggr.push_back(incr);
        return node->sibling ? construct(tree, node->sibling) : tree->rework(dim);
    }
};


class TokenManager {
private:
    map<string, dataDescript*> table;
    typedef pair<dataDescript*, dataDescript*> ddPair;
    typedef vector<ddPair> Record;
    stack< Record > record;
    int tempNum, globalNum, paramNum, cpNum; /* #t, #T, #l*/
    
public:
    TokenManager(): tempNum(0), globalNum(0), paramNum(0), cpNum(0) { assert(record.empty()); }
    string newt() { string t = "t" + to_string(tempNum); tempNum++; return t;}
    string newT() { string T = "T" + to_string(globalNum); globalNum++; return T;}
    string newp() { string p = "p" + to_string(paramNum); paramNum++; return p;}
    string newl() { string l = "l" + to_string(cpNum); cpNum++; return l;}
    void ascend() { record.push({}); paramNum = 0; }
    void insert(dataDescript* dd, bool param = false) {
        if (param)
            dd->eeyore = newp();
        else
            dd->eeyore = newT();
        if (table.find(dd->token) != table.end()) {
            dataDescript* old = table[dd->token];
            record.top().push_back( ddPair(old, dd));
        }
        else {
            record.top().push_back( ddPair(NULL, dd));
        }
        table[dd->token] = dd;
    }
    void descend() {
        for (auto item: record.top())
            table[item.second->token] = item.first;
        record.pop();
    }

    int query(string token) { return table[token]->eval(); }
    int query(string token, vector<int> &addr) { return table[token]->eval(addr); }
    
    string getEeyore(string token) { return table[token]->eeyore; } /* return correspoding token in eeyore */
    vector<int>& getDim(string token) { return table[token]->dim; } /* return dim vector of given token */
    dataCell* getInits(string token) { return table[token]->inits; } /* return inits tree of given token */

    void instantialize(string token) {
        /* convert constant expr to integer */
        assert(table.find(token) != table.end());
        assert(table[token] != NULL);
        vector<int> &dim = table[token]->dim;
        dataCell *inits = table[token]->inits;
        if (inits) inits->instantialize();
    }
    void initialize(string token, bool var) {
        /* do original assignment */
        assert(table.find(token) != table.end());
        assert(table[token] != NULL);
        vector<int> &dim = table[token]->dim;
        dataCell *inits = table[token]->inits;
        string eeyore = table[token]->eeyore;
        if (inits) inits->initialize(eeyore, dim, 0, var);
    }
};