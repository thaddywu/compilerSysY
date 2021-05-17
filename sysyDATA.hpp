#include <bits/stdc++.h>
#ifndef sysyAST_hpp
#include "sysyAST.hpp"
#define sysyAST_hpp
#endif
using namespace std;
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
    virtual void initialize(string &eeyore, vector<int> &shape, int addr, bool var) { assert(false); }
};
class dataAggr: public dataCell{
public:
    vector<dataCell*> aggr;
    dataAggr(int _depth): dataCell(_depth) { assert(aggr.empty()); }
    void condense(vector<int> &shape) ;
    void liftup(vector<int> &shape, int depth) ;
    void merge(vector<int> &shape) ;
    dataAggr* rework(vector<int> &shape) ;
    virtual void instantialize() ;
    virtual void initialize(string &eeyore, vector<int> &shape, int addr, bool var) ;
    virtual void debug() ;

};
class dataLeaf: public dataCell{
public:
    sysyAST *expr;
    dataLeaf(int _depth, sysyAST *_expr): dataCell(_depth), expr(_expr) {}
    virtual void debug() { if (expr) printf("%d", expr->eval() ); else printf("x"); }
    virtual void instantialize() { if (expr) expr = new _INTEGER(expr->eval()); }
    virtual void initialize(string &eeyore, vector<int> &shape, int addr, bool var) ;
};

extern void zero_padding(dataCell* &root, vector<int> &shape, int depth);