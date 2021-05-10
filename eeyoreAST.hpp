#include <bits/stdc++.h>
using namespace std;

void print(string x) ;
void printTab(string x) ;

class eeyoreAST {
public:
    eeyoreAST() {}
    ~eeyoreAST() {}
    virtual void Dump() { assert(false); } // printCode
    virtual string getName() { assert(false); }
};

/*
    Definition of nodes in Abstraction Syntax Tree of eeyore
*/
class _eATOM: public eeyoreAST {
public:
    _eATOM() {}
};
class _eNUM: public _eATOM {
public:
    int num;
    _eNUM(int _num): num(_num) {}
    virtual string getName() { return to_string(num); }
};
class _eVAR: public _eATOM {
public:
    string var;
    _eVAR(string _var): var(_var) {}
    virtual string getName() { return var; }
};

class _eDEFVAR: public eeyoreAST {
public:
    string var;
    _eDEFVAR(string _var): var(_var) {}
    virtual void Dump() { printTab("var " + var); }
};
class _eDEFARR: public eeyoreAST {
public:
    string var; int size; // without * 4
    _eDEFARR(string _var, int _size): var(_var), size(_size) {}
    virtual void Dump() { printTab("var " + to_string(size * 4) + " " + var); }
};
class _eDIRECT: public eeyoreAST {
public:
    eeyoreAST *a, *t;
    _eDIRECT(eeyoreAST *_a, eeyoreAST *_t): a(_a), t(_t) {}
    virtual void Dump() { printTab(a->getName() + " = " + t->getName()); }
};
class _eUNARY: public eeyoreAST {
public:
    eeyoreAST *a, *t; string op;
    _eUNARY(eeyoreAST *_a, string _op, eeyoreAST *_t): a(_a), op(_op), t(_t) {}
    virtual void Dump() { printTab(a->getName() + " = " + op + t->getName()); }
};
class _eBINARY: public eeyoreAST {
public:
    eeyoreAST *a, *t1, *t2; string op;
    _eBINARY(eeyoreAST *_a, eeyoreAST *_t1, string _op, eeyoreAST *_t2): a(_a), t1(_t1), op(_op), t2(_t2) {}
    virtual void Dump() { printTab(a->getName() + " = " + t1->getName() + " " + op + " " + t2->getName()); }
};
class _eLOAD: public eeyoreAST {
public:
    eeyoreAST *a, *x; string t;
    _eLOAD(eeyoreAST *_a, string _t, eeyoreAST *_x): a(_a), t(_t), x(_x) {}
    virtual void Dump() { printTab(a->getName() + " = " + t + "[" + x->getName() + "]"); }
};
class _eSTORE: public eeyoreAST {
public:
    eeyoreAST *t, *x; string a;
    _eSTORE(string _a, eeyoreAST *_x, eeyoreAST *_t): a(_a), x(_x), t(_t) {}
    virtual void Dump() { printTab(a + "[" + x->getName() + "] = " + t->getName()); }
};
class _eFUNCRET: public eeyoreAST {
public:
    eeyoreAST *a; string func;
    _eFUNCRET(eeyoreAST *_a, string _func): a(_a), func(_func) {}
    virtual void Dump() { printTab(a->getName() + " = call " + func); }
};
class _eIFGOTO: public eeyoreAST {
public:
    eeyoreAST *t1, *t2; string l; string op;
    _eIFGOTO(eeyoreAST *_t1, string _op, eeyoreAST *_t2, string _l): t1(_t1), op(_op), t2(_t2), l(_l) {}
    virtual void Dump() { printTab("if " + t1->getName() + " " + op + " " + t2->getName() + " goto " + l); }
};
class _eGOTO: public eeyoreAST {
public:
    string l;
    _eGOTO(string _l): l(_l) {}
    virtual void Dump() { printTab("goto " + l); }
};
class _ePARAM: public eeyoreAST {
public:
    eeyoreAST *t;
    _ePARAM(eeyoreAST *_t): t(_t) {}
    virtual void Dump() { printTab("param " + t->getName()); }
};
class _eLABEL: public eeyoreAST {
public:
    string l;
    _eLABEL(string _l): l(_l) {}
    virtual void Dump() { print(l + ":"); }
};
class _eRETVOID: public eeyoreAST {
public:
    _eRETVOID() {}
    virtual void Dump() { printTab("return"); }
};
class _eRET: public eeyoreAST {
public:
    eeyoreAST *t;
    _eRET(eeyoreAST *_t): t(_t) {}
    virtual void Dump() { printTab("return " + t->getName()); }
};
class _eFUNC: public eeyoreAST {
public:
    eeyoreAST *body; string func; int arity;
    _eFUNC(string _func, int _arity, eeyoreAST *_body): func(_func), arity(_arity), body(_body) {}
    virtual void Dump() { print(func + " [" + to_string(arity) + "]"); body->Dump(); print("end " + func); }
};
class _eCALL: public eeyoreAST {
public:
    string func;
    _eCALL(string _func): func(_func) {}
    virtual void Dump() { printTab("call " + func); }
};
class _eSEQ: public eeyoreAST {
public:
    vector<eeyoreAST *> seq;
    _eSEQ(vector<eeyoreAST *> _seq): seq(_seq) {}
    virtual void Dump() { for (auto stmt: seq) stmt->Dump(); }
};