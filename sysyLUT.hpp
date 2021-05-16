#include <bits/stdc++.h>
#ifndef eeyoreAST_hpp
#include "eeyoreAST.hpp"
#define eeyoreAST_hpp
#endif
#ifndef sysyAST_hpp
#include "sysyAST.hpp"
#define sysyAST_hpp
#endif
#ifndef sysyDATA_hpp
#include "sysyDATA.hpp"
#define sysyDATA_hpp
#endif

using namespace std;


extern int flatten(vector<int> &shape); // defined in sysy.y
extern void eeyoreStmt(eeyoreAST *x);
extern void eeyoreDecl(eeyoreAST *x);

/* ====================================================== */
/* dataDescript                                           */
/*      - name: array/var's name                         */
/*      - eeyore: name after translation                  */
/*      - shape: shape of array, shape[i] for length of shape-i */
/*           for var, it's empty                          */
/*      - inits: data, type of _DATA_CELL                 */
/*      - eval(addr): return value array's given position */
/*      - eval(): return value of var                     */
/*      - getSize(): get occupied memory size without * 4 */
/* ====================================================== */
class dataDescript {
public:
    string name;
    string eeyore; /* converted name in eeyore */
    vector<int> shape;
    dataCell *inits;
    dataDescript(string _name, vector<int> _shape, _TREE *_node);

    int eval(vector<int> &addr) ;
    int eval() { return inits ? ((dataLeaf *)inits)->expr->eval() : 0; }
    int getSize() { return flatten(shape); }
private:
    dataCell* construct(dataAggr *tree, _TREE *node) ;
};

/* ============================================================ */
/* VarManager                                                 */
/*      - table: map for name->dataDescript                    */
/*      - record: declaration records in this domain            */
/*      - newp(), newt(), newT(), newl(): new var or annotation */
/*      - newEnviron(): create new environ                      */
/*      - deleteEnviron(): back to last environ                 */
/*      - insert(dd, param): insert a var/array                 */
/*          - dd: dataDescript                                  */
/*          - param: boolean, to determine p0 or T0             */
/*      - getEeyore(name): atomize name                        */
/*      - getshape(name): return its shape                       */
/*      - getSize(name): return its flattened size             */
/*          - no definition for var                             */
/*      - initialize(name): initialize                         */
/*      - instantialize(name): optimization                    */
/* ============================================================ */
class VarManager {
private:
    map<string, dataDescript*> table;
    typedef pair<dataDescript*, dataDescript*> ddPair;
    typedef vector<ddPair> Record;
    stack< Record > record;
    int tempNum, varNum, paramNum, labelNum; /* #t, #T, #p, #l*/
    
public:
    VarManager(): tempNum(0), varNum(0), paramNum(0), labelNum(0) { assert(record.empty()); }
    string newt() { string t = "t" + to_string(tempNum); tempNum++; eeyoreDecl(new _eDEFVAR(t)); return t;}
    string newT() { string T = "T" + to_string(varNum); varNum++; eeyoreDecl(new _eDEFVAR(T)); return T;}
    string newT(int sz) { string T = "T" + to_string(varNum); varNum++; eeyoreDecl(new _eDEFARR(T, sz)); return T;}
    string newp() { string p = "p" + to_string(paramNum); paramNum++; return p;}
    string newl() { string l = "l" + to_string(labelNum); labelNum++; return l;}
    void newEnviron() { record.push({}); paramNum = 0; /* not-good implementation here */ }
    void insert(dataDescript* dd, bool param = false) ;
    void deleteEnviron() ;

    int query(string name) { return table[name]->eval(); }
    int query(string name, vector<int> &addr) { return table[name]->eval(addr); }
    
    string getEeyore(string name) { return table[name]->eeyore; } /* return correspoding name in eeyore */
    vector<int>& getshape(string name) { return table[name]->shape; } /* return shape vector of given name */
    int getSize(string name) { return table[name]->getSize(); }

    void instantialize(string name) ;
    void initialize(string name, bool var, bool zero_pad) ;
};

/* ============================================================ */
/* FuncManager                                                  */
/*      - table: map for name->shape                           */
/*      - insert: registeration a function                      */
/*      - query(name): return its shape                        */
/* ============================================================ */
class FuncManager {
public:
    map<string, vector<int> > table;
    FuncManager() ;
    int insert(string name, sysyAST *param) ;
    vector<int>& query(string name) { return table[name]; }
};