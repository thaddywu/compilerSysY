#include <bits/stdc++.h>
#ifndef eeyoreAST_hpp
#include "eeyoreAST.hpp"
#define eeyoreAST_hpp
#endif
using namespace std;

class sysyAST {
public:
    sysyAST() {}
    ~sysyAST() {}
        // _STR::getToken return string
    virtual string getToken() { assert(false); }
        // _EXPR::eval return its integer value, 
    virtual int eval() { assert(false); }
        // _EXPR::atomize return its representation which could appear in the right of assignment. special case : true || array[-1] should not incur error
    virtual eeyoreAST* atomize() { assert(false); }
        // For _ADDR_LIST, corresponding function name is passed
    virtual eeyoreAST* atomize(string name) { assert(false); }
        // symbol: For expr, return its operator
    virtual string symbol() { assert(false); }
        // _ADDR_LIST::vectorize embed a _ADDR_LIST into a list
    virtual void vectorize(vector<int> &v) { assert(false); }
        // _CALL_LIST::pass pass params
    virtual void pass() { assert(false); }
        /* _DECL structure:
            instantialize: _DEF_CONST_VAR, _DEF_CONST_ARR calls varManager/ other _DECL type do nothing
                to substitute expression with its integer value.
            initialize: for all, implemented in _DEF_VAR & _DEF_ARR
        */
    virtual void instantialize() { assert(false); }
    virtual void initialize() { assert(false); }
        // _PARAM_VAR, _PARAM_ARR, _VAR, _ARRAY_ITEM::: isvar, return a boolean 
    virtual int isvar() { assert(false); }

        /* translate: translate into Abstraction Syntax Tree of eeyore form
            - ctn: continue to where
            - brk: break to where
            - glb: if now in function
        */
    virtual void translate(string ctn, string brk, bool glb) { assert(false); }

    /* some virtual functions are only declared in _TREE,
        so as to avoid excessive declaration in based type sysyAST.
        As a result, coercion is needed when calling those functions.
    */
};

/*
    Definition of nodes in Abstraction Syntax Tree of sysyY
    _PROGRAM:
        program.. top module
    _EXPR:
        - _BINARY_OP: lop, rop
            - _ADD, _SUB, _DIV, _MUL, _MOD, _AND, _OR, _LT, _GT, _LE, _GE, _EQ, _NEQ
        - _UNARY_OP: op
            - _NEG, _NOT
        - _INTEGER: $num$
        - _VAR: $name$
        - _FUNC_CALL: $name$ param
        - _ARRAY_ITEM: $name$ param
    
    _LIST: head, tail
        - _CALL_LIST: head, tail | head
        - _ADDR_LIST: [head] tail | [head]
        - _PARAM_LIST: int head, tail | int head
        - _STMT_SEQ: head tail
    
    _STMT:
        - _IF: if (cond) body
        - _IF_ELSE: if (cond) body else ebody
        - _WHILE: while (cond) body
        - _RETURN_VOID: return ;
        - _RETURN_EXPR: return expr;
        - _CONTINUE: continue;
        - _ASSGIN: lval = rval;
        - _FUNC: (int | void) $name$ (param) {body}
        - _DECL: $name$ = expr
            - _DEF_VAR: $name$ = expr
                - _DEF_CONST_VAR: $name$ = expr
                - _PARAM_VAR: in function declaration
            - _DEF_ARR: $name$ addr = magic
                - _DEF_CONST_ARR: $name$ addr = magic
                - _PARAM_ARR: in function declaration
        - _BLOCK: { .. }

    _TREE: rval of constant definition
        - _TREE_NODE: child, sibling
        - _TREE_LEAF: expr

    _STRING:  $name$
    _DUMMY: non-sense
*/

class _STRING: public sysyAST {
public:
    string name;
    _STRING(string _name): name(_name) {}
    virtual string getToken() { return name; }
};
class _DUMMY: public sysyAST {
    /* this class is necessary, designed for
        empty statement or expr statement */
public:
    string name;
    virtual void translate(string ctn, string brk, bool glb) { } /* do nothing */
};

class _PROGRAM: public sysyAST {
public:
    sysyAST *program;
    _PROGRAM(sysyAST *_program): program(_program) {}
    virtual void translate(string ctn, string brk, bool glb) ;
};
/* ==================================== */
/*                 _EXPR                */
/* ==================================== */
class _EXPR: public sysyAST {
public:
    _EXPR() {}
    void translate(string ctn, string brk, bool glb) ;
};
class _UNARY_OP: public _EXPR {
public:
    sysyAST *op;
    _UNARY_OP(sysyAST *_op): op(_op) {}
    virtual eeyoreAST* atomize() ;
};
class _BINARY_OP: public _EXPR {
public:
    sysyAST *lop, *rop;
    _BINARY_OP(sysyAST *_lop, sysyAST *_rop): lop(_lop), rop(_rop) {}
    virtual eeyoreAST* atomize() ;
};
class _NEG: public _UNARY_OP {
public:
    _NEG(sysyAST *_op): _UNARY_OP(_op) {}
    virtual int eval() { return - op->eval(); }
    virtual string symbol() { return "-"; }
};
class _NOT: public _UNARY_OP {
public:
    _NOT(sysyAST *_op): _UNARY_OP(_op) {}
    virtual int eval() { return ! op->eval(); }
    virtual string symbol() { return "!"; }
};
class _ADD: public _BINARY_OP {
public:
    _ADD(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() + rop->eval(); }
    virtual string symbol() { return "+"; }
};
class _SUB: public _BINARY_OP {
public:
    _SUB(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() - rop->eval(); }
    virtual string symbol() { return "-"; }
};
class _MUL: public _BINARY_OP {
public:
    _MUL(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() * rop->eval(); }
    virtual string symbol() { return "*"; }
};
class _DIV: public _BINARY_OP {
public:
    _DIV(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() / rop->eval(); }
    virtual string symbol() { return "/"; }
};
class _MOD: public _BINARY_OP {
public:
    _MOD(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() % rop->eval(); }
    virtual string symbol() { return "\%"; }
};
class _AND: public _BINARY_OP {
public:
    _AND(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() && rop->eval(); }
    virtual string symbol() { return "&&"; }
    virtual eeyoreAST* atomize() ;
};
class _OR: public _BINARY_OP {
public:
    _OR(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() || rop->eval(); }
    virtual string symbol() { return "||"; }
    virtual eeyoreAST* atomize() ;
};
class _LT: public _BINARY_OP {
public:
    _LT(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() < rop->eval(); }
    virtual string symbol() { return "<"; }
};
class _LE: public _BINARY_OP {
public:
    _LE(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() <= rop->eval(); }
    virtual string symbol() { return "<="; }
};
class _GT: public _BINARY_OP {
public:
    _GT(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() > rop->eval(); }
    virtual string symbol() { return ">"; }
};
class _GE: public _BINARY_OP {
public:
    _GE(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() >= rop->eval(); }
    virtual string symbol() { return ">="; }
};
class _EQ: public _BINARY_OP {
public:
    _EQ(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() == rop->eval(); }
    virtual string symbol() { return "=="; }
};
class _NEQ: public _BINARY_OP {
public:
    _NEQ(sysyAST *_lop, sysyAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() != rop->eval(); }
    virtual string symbol() { return "!="; }
};
class _INTEGER: public _EXPR {
public:
    int num;
    _INTEGER(int _num): num(_num) {}
    virtual int eval() { return num; }
    virtual eeyoreAST* atomize() ;
};
class _VAR: public _EXPR {
public:
    string name;
    _VAR(string _name): name(_name) {}
    virtual int eval();
    virtual eeyoreAST* atomize() ;
    virtual int isvar() { return 1; }
};
class _FUNC_CALL: public _EXPR {
public:
    string name; sysyAST *param;
    _FUNC_CALL(string _name, sysyAST *_param): name(_name), param(_param) {}
    virtual eeyoreAST* atomize() ;
    virtual void translate(string ctn, string brk, bool glb);
    virtual void pass() ;
};
class _ARRAY_ITEM: public _EXPR {
public:
    string name; sysyAST *param;
    _ARRAY_ITEM(string _name, sysyAST *_param): name(_name), param(_param) {}
    virtual int eval();
    virtual eeyoreAST* atomize() ;
    virtual int isvar() { return 0; }
};


/* ==================================== */
/*                 _LIST                */
/* ==================================== */
class _LIST: public sysyAST {
public:
    sysyAST *head, *tail;
    _LIST(sysyAST *_head, sysyAST *_tail): head(_head), tail(_tail) {}
};
class _CALL_LIST: public _LIST {
public:
    _CALL_LIST(sysyAST *_head, sysyAST *_tail): _LIST(_head, _tail) {}
};
class _ADDR_LIST: public _LIST {
public:
    _ADDR_LIST(sysyAST *_head, sysyAST *_tail): _LIST(_head, _tail) {}
    virtual void vectorize(vector<int> &v) {
        if (head) v.push_back(head->eval());
        if (tail) tail->vectorize(v);
    }
    virtual eeyoreAST* atomize(string name) ;
    virtual eeyoreAST* atomize() { assert(false); }
};
class _PARAM_LIST: public _LIST {
public:
    _PARAM_LIST(sysyAST *_head, sysyAST *_tail): _LIST(_head, _tail) {}
    virtual void translate(string ctn, string brk, bool glb);
    virtual eeyoreAST* atomize() { assert(false); }
};
class _STMT_SEQ: public _LIST {
public:
    _STMT_SEQ(sysyAST *_head, sysyAST *_tail): _LIST(_head, _tail) {}
    virtual void translate(string ctn, string brk, bool glb);
};


/* ==================================== */
/*                 _STMT                */
/* ==================================== */
class _STMT: public sysyAST {
public:
    _STMT() {}
};
class _IF: public _STMT {
public:
    sysyAST *cond, *body;
    _IF(sysyAST *_cond, sysyAST *_body): cond(_cond), body(_body) {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _IF_ELSE: public _STMT {
public:
    sysyAST *cond, *body, *ebody;
    _IF_ELSE(sysyAST *_cond, sysyAST *_body, sysyAST *_ebody): cond(_cond), body(_body), ebody(_ebody) {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _WHILE: public _STMT {
public:
    sysyAST *cond, *body;
    _WHILE(sysyAST *_cond, sysyAST *_body): cond(_cond), body(_body) {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _RETURN_VOID: public _STMT {
public:
    _RETURN_VOID() {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _RETURN_EXPR: public _STMT {
public:
    sysyAST *expr;
    _RETURN_EXPR(sysyAST *_expr): expr(_expr) {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _CONTINUE: public _STMT {
public:
    _CONTINUE() {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _BREAK: public _STMT {
public:
    _BREAK() {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _ASSIGN: public _STMT {
public:
    sysyAST *lop, *rop;
    _ASSIGN(sysyAST *_lop, sysyAST *_rop): lop(_lop), rop(_rop) {}
    virtual void translate(string ctn, string brk, bool glb);
};
class _BLOCK: public _STMT {
public:
    sysyAST *block;
    _BLOCK(sysyAST *_block): block(_block) {}
    virtual void translate(string ctn, string brk, bool glb);
};


/* ==================================== */
/*                 _FUNC                */
/* ==================================== */
class _FUNC: public _STMT {
public:
    string name; sysyAST *param, *body; bool isvoid;
    _FUNC(string _name, sysyAST *_param, sysyAST *_body, bool _isvoid): name(_name), param(_param), body(_body), isvoid(_isvoid) {}
    virtual void translate(string ctn, string brk, bool glb);
};

/* ==================================== */
/*                 _DECL                */
/* ==================================== */
class _DECL: public sysyAST {
public:
    _DECL() {}
    virtual void initialize() { assert(false); }
    virtual void instantialize() { assert(false); } 
};
class _DEF_VAR: public _DECL {
public:
    string name; sysyAST *inits;
    _DEF_VAR(string _name, sysyAST *_inits): name(_name), inits(_inits) {}
    virtual void translate(string ctn, string brk, bool glb);
    virtual void initialize() ;
    virtual void instantialize() {} /* do nothing */
};
class _DEF_CONST_VAR: public _DEF_VAR {
public:
    _DEF_CONST_VAR(string _name, sysyAST *_inits): _DEF_VAR(_name, _inits) {}
    virtual void initialize() ;
    virtual void instantialize() ;
};
class _PARAM_VAR: public _DEF_VAR {
public:
    _PARAM_VAR(string _name, sysyAST *_inits): _DEF_VAR(_name, _inits) {}
    virtual void translate(string ctn, string brk, bool glb);
    virtual void initialize() { assert(false); } /* redundant assertion */
    virtual void instantialize() {} /* do nothing */
    virtual int isvar() { return 1; }
};

class _DEF_ARR: public _DECL {
public:
    string name; sysyAST *addr, *inits;
    _DEF_ARR(string _name, sysyAST *_addr, sysyAST *_inits): name(_name), addr(_addr), inits(_inits) {}
    virtual void translate(string ctn, string brk, bool glb);
    virtual void initialize() ;
    virtual void instantialize() {} /* do nothing */
};
class _DEF_CONST_ARR: public _DEF_ARR {
public:
    _DEF_CONST_ARR(string _name, sysyAST *_addr, sysyAST *_inits): _DEF_ARR(_name, _addr, _inits) {}
    virtual void initialize() ;
    virtual void instantialize() ;
};
class _PARAM_ARR: public _DEF_ARR {
public:
    _PARAM_ARR(string _name, sysyAST *_addr, sysyAST *_inits): _DEF_ARR(_name, _addr, _inits) {}
    virtual void translate(string ctn, string brk, bool glb);
    virtual void initialize() { assert(false); } /* redundant assertion */
    virtual void instantialize() {} /* do nothing */
    virtual int isvar() { return 0; }
};

/* ==================================== */
/*                 _TREE                */
/* ==================================== */
class _TREE: public sysyAST {
public:
    _TREE *sibling;
    _TREE(): sibling(NULL) {}
    virtual bool leaf() { assert(false); }
    virtual sysyAST* getExpr() { assert(false); }
    virtual _TREE* getChild() { assert(false); }
    virtual void debug(int ws = 0) {}
};
class _TREE_NODE: public _TREE {
public:
    _TREE *child;
    _TREE_NODE(_TREE *_child): _TREE(), child(_child) {}
    virtual bool leaf() { return false; }
    virtual _TREE* getChild() { return child; }
    virtual void debug(int ws = 0) {
        for (int i = 1; i <= ws; i++) printf(" ");
        printf("-\n");
        child->debug(ws + 1);
        if (sibling) sibling->debug(ws);
    }
};
class _TREE_LEAF: public _TREE {
public:
    sysyAST *expr;
    _TREE_LEAF(sysyAST *_expr): _TREE(), expr(_expr) {}
    virtual bool leaf() { return true; }
    virtual sysyAST* getExpr() { return expr; }
    virtual void debug(int ws = 0) {
        for (int i = 1; i <= ws; i++) printf(" ");
        printf("unit\n");
        if (sibling) sibling->debug(ws);
    }
};