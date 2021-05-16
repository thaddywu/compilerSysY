#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

/* =============================================== */
/* condese                                         */
/*      - merge nodes of the same depth            */
/*      - at most n nodes be merged once, n:length */
/* =============================================== */
void dataAggr::condense(vector<int> &shape) {
    assert(!aggr.empty());
    int depth = aggr.back()->depth;
    assert(depth > 0);
    dataAggr *cds = new dataAggr(depth - 1);
    for (int k = 0; k < shape[depth - 1]; k++)
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
void dataAggr::merge(vector<int> &shape) {
    while (!aggr.empty()) {
        int depth = aggr.back()->depth;
        if (depth == 0) {
            assert(aggr.size() == 1);
            return ;
        }
        int sz = aggr.size(), n = shape[depth - 1];
        if (sz < n || aggr[sz-n]->depth != depth) return ;
        condense(shape);
    }
}
/* =============================================== */
/* liftup                                          */
/*      - extend current array into higher-shape     */
/*      - remember to merge nodes of the same      */
/*          depth immediately after liftup once    */
/* =============================================== */
void dataAggr::liftup(vector<int> &shape, int depth) {
    while (!aggr.empty() && aggr.back()->depth > depth)
        { condense(shape); merge(shape); }
}
dataAggr* dataAggr::rework(vector<int> &shape) {
    /* generate the final node after processing with all siblings, 
        and return its pointer */
    assert(!aggr.empty());
    while (aggr[0]->depth != aggr.back()->depth)
        condense(shape);
    depth = aggr[0]->depth - 1;
    return this;
}
/* =============================================== */
/* instantialize                                   */
/*      - substitue expr with its integer value    */
/* =============================================== */
void dataAggr::instantialize() {
    for (auto child: aggr)
        child->instantialize();
}
/* =============================================== */
/* initialize                                      */
/*      - set initial value                        */
/* =============================================== */
void dataAggr::initialize(string &eeyore, vector<int> &shape, int addr, bool var) {
    addr *= shape[depth];
    for (int i = 0; i < aggr.size(); i++)
        aggr[i]->initialize(eeyore, shape, addr + i, var);
}
void dataLeaf::initialize(string &eeyore, vector<int> &shape, int addr, bool var) {
    if (expr == NULL) return ;
    eeyoreAST *t = expr->atomize();
    if (var)
        eeyoreStmt(new _eDIRECT(new _eVAR(eeyore), t));
    else
        eeyoreStmt(new _eSAVE(eeyore, new _eNUM(addr * 4), t));
}
/* =============================================== */
/* debug                                           */
/*      - visualization                            */
/* =============================================== */
void dataAggr::debug() {
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
/* zero-padding                                    */
/*      - const var shoule be zero-padded          */
/* =============================================== */
void zero_padding(dataCell* &root, vector<int> &shape, int depth) {
    if (depth == shape.size()) {
        /* leaf node */
        if (!root)
            root = (dataCell *)(new dataLeaf(depth, new _INTEGER(0)));
        return ;
    }
    if (!root)
        root = (dataCell *)(new dataAggr(depth));
    dataAggr *x = (dataAggr *) root;
    for (auto &child: x->aggr)
        zero_padding(child, shape, depth + 1);
    for (int i = x->aggr.size(); i < shape[depth]; i++) {
        dataCell *child = NULL;
        zero_padding(child, shape, depth + 1);
        x->aggr.push_back(child);
    }
}