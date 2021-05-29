
#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

/* ====================================================== */
/* dataDescript                                           */
/*      - declaration                                     */
/* ====================================================== */
dataDescript::dataDescript(string _name, vector<int> _shape, _TREE *_node, bool _isconst):
name(_name), shape(_shape), isconst(_isconst), inits(NULL) {
    int length = shape.size();
    if (length > 0) {
        shape_all.resize(length);
        shape_all[length - 1] = 1;
        for (int i = length - 2; i >= 0; i--)
            shape_all[i] = shape_all[i + 1] * shape[i + 1];
    }
    /* the total length of dim i */
    if (_node == NULL) return ;
    if (_node->leaf())
        inits = (dataCell *) new dataLeaf(shape.size(), _node->getExpr());
    else {
        assert(_node->getSibling() == NULL);
        inits = construct(new dataAggr(-1), _node->getChild());
    }
    while (inits->depth > 0) {
        dataAggr *anc = new dataAggr(inits->depth - 1);
        anc->aggr.push_back(inits);
        inits = anc;
    }
}
/* ====================================================== */
/* dataDescript::eval(addr)                               */
/*      - return value array's given position             */
/* ====================================================== */
int dataDescript::eval(vector<int> &addr) {
    if (inits == NULL) return 0;
    dataAggr *node = (dataAggr *)inits;
    for (auto x: addr) {
        if (node->aggr.size() < x) return 0;
        if (!node->aggr[x]) return 0;
        node = (dataAggr *)node->aggr[x];
    }
    return ((dataLeaf *)node)->expr->eval();
}

/* ====================================================== */
/* dataDescript::construct(tree, node)                    */
/*      - private internal function                       */
/*      - construct dataAggr from original _TREE          */
/* ====================================================== */
dataCell* dataDescript::construct(dataAggr *tree, _TREE *node) {
    assert(node != NULL);
    dataCell *incr;
    if (node->leaf())
        incr = (dataCell*) new dataLeaf(shape.size(), node->getExpr());
    else
        incr = construct(new dataAggr(-1), node->getChild());

    tree->liftup(shape, incr->depth);
    tree->aggr.push_back(incr);
    return node->sibling ? construct(tree, node->sibling) : tree->prune(shape);
}

/* ====================================================== */
/* VarManager::insert(dd, param    )                    */
/*      - given data-descipt, register for var            */
/*      - param, if it is param in function               */
/* ====================================================== */
void VarManager::insert(dataDescript* dd, bool param) {
    if (param)
        dd->eeyore = newp();
    else
        dd->eeyore = dd->shape.empty() ? newT(dd->isconst) : newT(flatten(dd->shape), dd->isconst);
    /* decl print in newT */
    if (table.find(dd->name) != table.end()) {
        dataDescript* old = table[dd->name];
        record.top().push_back( ddPair(old, dd));
    }
    else {
        record.top().push_back( ddPair(NULL, dd));
    }
    table[dd->name] = dd;
}
/* ====================================================== */
/* VarManager::deleteEnviron                            */
/*      - exit a function, and delete its environ         */
/*      - vars with multiple names outside this function  */
/*        is now restored                                 */
/* ====================================================== */
void VarManager::deleteEnviron() {
    for (auto item: record.top())
        table[item.second->name] = item.first;
    record.pop();
}
/* ====================================================== */
/* VarManager::instantialize                            */
/*      - convert constant expr to integer                */
/*      - called after var inserted into varManager     */
/* ====================================================== */
void VarManager::instantialize(string name) {
    /* convert constant expr to integer */
    assert(table.find(name) != table.end());
    assert(table[name] != NULL);
    vector<int> &shape = table[name]->shape;
    dataCell *inits = table[name]->inits;
    if (inits) inits->instantialize();
}
/* ====================================================== */
/* VarManager::initialization                             */
/*      - initialize the vars                             */
/*      - called after var inserted into varManager       */
/*      - zero-pad: for global vars, 0 padded when flag on*/
/* ====================================================== */
void VarManager::initialize(string name, bool var, bool zero_pad) {
    /* do original assignment */
    assert(table.find(name) != table.end());
    assert(table[name] != NULL);
    vector<int> &shape = table[name]->shape;
    dataCell *&inits = table[name]->inits;
    string eeyore = table[name]->eeyore;
    if (zero_pad) zero_padding(inits, shape, 0);
    if (inits) inits->initialize(eeyore, shape, 0, var);
}

/* ====================================================== */
/* FuncManager::FuncManager                               */
/*      - runtime function should be registered here      */
/* ====================================================== */
FuncManager::FuncManager() {
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
    table["_sysy_starttime"] = univar;
    table["_sysy_stoptime"] = univar;
}
/* ====================================================== */
/* FuncManager::insert(name, param                       */
/*      - register a function                             */
/* ====================================================== */
int FuncManager::insert(string name, sysyAST *param) {
    assert(table.find(name) == table.end());
    vector<int> plist {};
    if (param != NULL) {
        _PARAM_LIST *cur = (_PARAM_LIST *) param;
        while (cur != NULL) {
            plist.push_back(cur->head->isvar());
            cur = (_PARAM_LIST *) (cur->tail);
        }
    }
    table[name] = plist;
    return plist.size();
}