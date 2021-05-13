#include <bits/stdc++.h>
#ifndef eeyoreAST_hpp
#include "eeyoreAST.hpp"
#define eeyoreAST_hpp
#endif
#ifndef sysyAST_hpp
#include "sysyAST.hpp"
#define sysyAST_hpp
#endif

using namespace std;


extern int flatten(vector<int> &dim); // defined in sysy.y
extern void eeyoreStmt(eeyoreAST *x);
extern void eeyoreDecl(eeyoreAST *x);

/*
    dataCell:
        - dataLeaf: leaf node of data tree, expr represents its value.
        - dataAggr: internal node of data tree, aggr stores its children's ptrs
*/
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
/* =============================================== */
/* condese                                         */
/*      - merge nodes of the same depth            */
/*      - at most n nodes be merged once, n:length */
/* =============================================== */
    void condense(vector<int> &dim) {
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
/* =============================================== */
/* merge                                           */
/*      - merge nodes of the same depth            */
/*      - only exactly n nodes                     */
/* =============================================== */
    void merge(vector<int> &dim) {
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
/* =============================================== */
/* liftup                                          */
/*      - extend current array into higher-dim     */
/*      - remember to merge nodes of the same      */
/*          depth immediately after liftup once    */
/* =============================================== */
    void liftup(vector<int> &dim, int depth) {
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
/* =============================================== */
/* instantialize                                   */
/*      - substitue expr with its integer value    */
/* =============================================== */
    virtual void instantialize() {
        for (auto child: aggr)
            child->instantialize();
    }
/* =============================================== */
/* initialize                                      */
/*      - set initial value                        */
/* =============================================== */
    virtual void initialize(string &eeyore, vector<int> &dim, int addr, bool var) {
        addr *= dim[depth];
        for (int i = 0; i < aggr.size(); i++)
            aggr[i]->initialize(eeyore, dim, addr + i, var);
    }
};
class dataLeaf: public dataCell{
public:
    sysyAST *expr;
    dataLeaf(int _depth, sysyAST *_expr): dataCell(_depth), expr(_expr) {}
    virtual void debug() { if (expr) printf("%d", expr->eval() ); else printf("x"); }
    virtual void instantialize() { if (expr) expr = new _INTEGER(expr->eval()); }
    virtual void initialize(string &eeyore, vector<int> &dim, int addr, bool var) {
        if (expr == NULL) return ;
        eeyoreAST *t = expr->atomize();
        if (var)
            eeyoreStmt(new _eDIRECT(new _eVAR(eeyore), t));
        else
            eeyoreStmt(new _eSAVE(eeyore, new _eNUM(addr * 4), t));
    }
};

/* ====================================================== */
/* dataDescript                                           */
/*      - token: array/var's name                         */
/*      - eeyore: name after translation                  */
/*      - dim: shape of array, dim[i] for length of dim-i */
/*           for var, it's empty                          */
/*      - inits: data, type of _DATA_CELL                 */
/*      - eval(addr): return value array's given position */
/*      - eval(): return value of var                     */
/*      - getSize(): get occupied memory size without * 4 */
/* ====================================================== */
class dataDescript {
public:
    string token;
    string eeyore; /* converted token in eeyore */
    vector<int> dim;
    dataCell *inits;
    dataDescript(string _token, vector<int> _dim, _TREE *_node):
        token(_token), dim(_dim), inits(NULL) { 
        if (_node == NULL) return ;
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
    int eval() { return inits ? ((dataLeaf *)inits)->expr->eval() : 0; }
    int getSize() { return flatten(dim); }
private:
    dataCell* construct(dataAggr *tree, _TREE *node) {
        assert(node != NULL);
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

/* ============================================================ */
/* TokenManager                                                 */
/*      - table: map for token->dataDescript                    */
/*      - record: declaration records in this domain            */
/*      - newp(), newt(), newT(), newl(): new var or annotation */
/*      - newEnviron(): create new environ                      */
/*      - deleteEnviron(): back to last environ                 */
/*      - insert(dd, param): insert a var/array                 */
/*          - dd: dataDescript                                  */
/*          - param: boolean, to determine p0 or T0             */
/*      - getEeyore(token): atomize name                      */
/*      - getDim(token): return its shape                       */
/*      - getSize(token): return its flattened size             */
/*          - no definition for var                             */
/*      - initialize(token)                                     */
/*      - instantialize(token)                                  */
/* ============================================================ */
class TokenManager {
private:
    map<string, dataDescript*> table;
    typedef pair<dataDescript*, dataDescript*> ddPair;
    typedef vector<ddPair> Record;
    stack< Record > record;
    int tempNum, varNum, paramNum, labelNum; /* #t, #T, #p, #l*/
    
public:
    TokenManager(): tempNum(0), varNum(0), paramNum(0), labelNum(0) { assert(record.empty()); }
    string newt() { string t = "t" + to_string(tempNum); tempNum++; eeyoreDecl(new _eDEFVAR(t)); return t;}
    string newT() { string T = "T" + to_string(varNum); varNum++; eeyoreDecl(new _eDEFVAR(T)); return T;}
    string newT(int sz) { string T = "T" + to_string(varNum); varNum++; eeyoreDecl(new _eDEFARR(T, sz)); return T;}
    string newp() { string p = "p" + to_string(paramNum); paramNum++; return p;}
    string newl() { string l = "l" + to_string(labelNum); labelNum++; return l;}
    void newEnviron() { record.push({}); paramNum = 0; /* not-good implementation here */ }
    void insert(dataDescript* dd, bool param = false) {
        if (param)
            dd->eeyore = newp();
        else
            dd->eeyore = dd->dim.empty() ? newT() : newT(flatten(dd->dim));
        /* decl print in newT */
        if (table.find(dd->token) != table.end()) {
            dataDescript* old = table[dd->token];
            record.top().push_back( ddPair(old, dd));
        }
        else {
            record.top().push_back( ddPair(NULL, dd));
        }
        table[dd->token] = dd;
    }
    void deleteEnviron() {
        for (auto item: record.top())
            table[item.second->token] = item.first;
        record.pop();
    }

    int query(string token) { return table[token]->eval(); }
    int query(string token, vector<int> &addr) { return table[token]->eval(addr); }
    
    string getEeyore(string token) { return table[token]->eeyore; } /* return correspoding token in eeyore */
    vector<int>& getDim(string token) { return table[token]->dim; } /* return dim vector of given token */
    int getSize(string token) { return table[token]->getSize(); }

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

class FuncManager {
public:
    map<string, vector<int> > table;
    FuncManager() {
        vector<int> uniarr {0};
        vector<int> univar {1};
        vector<int> novar {};
        vector<int> mix {1, 0};
        table["getint"] = novar;
        table["getch"] = novar;
        table["getarray"] = uniarr;
        table["putint"] = univar;
        table["putch"] = univar;
        table["putarray"] = mix;
    }
    int insert(string token, sysyAST *param) {
        assert(table.find(token) == table.end());
        vector<int> plist {};
        if (param != NULL) {
            _PARAM_LIST *cur = (_PARAM_LIST *) param;
            while (cur != NULL) {
                plist.push_back(cur->head->isvar());
                cur = (_PARAM_LIST *) (cur->tail);
            }
        }
        table[token] = plist;
        return plist.size();
    }
    vector<int>& query(string token) {
        return table[token];
    }
};