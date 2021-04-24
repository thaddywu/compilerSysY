#include <bits/stdc++.h>
using namespace std;

class nodeAST {
public:
    nodeAST() {}
    ~nodeAST() {}
    // _EXPR:: eval expression
    virtual int eval() { assert(false); }
    // _STR:: return string
    virtual string getToken() { assert(false); }
    // _ADDR_LIST:: vectorize list & eval
    virtual void vectorize(vector<int> &v) { assert(false); }
    /* some virtual functions are embedded in _TREE,
        so as to avoid enormous declaration in nodeAST,
        but therefore explicit conversion is needed,
        when using functions of _TREE
    */
};

/*
    Definition of nodes, so as to construct Abstraction Syntax Tree
    _EXPR:
        - _BINARY_OP: lop, rop
            - _AND, _SUB, _DIV, _MUL, _MOD, _AND, OR, _LT, _GT, _LE, _GE, _EQ, _NEQ
        - _UNARY_OP: op
            _NEG, _NOT
        - _INTEGER: $num$
        - _VAR: $token$
        - _FUNC_CALL: $token$ param
        - _ARRAY_ITEM: $token$ param
    
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
        - _FUNC: (int | void) $token$ (param) {body}
        - _DECL: $token$ = expr
            - _DEF_VAR: $token$ = expr
                - _DEF_CONST_VAR: $token$ = expr
            - _DEF_ARR: $token$ addr = magic
                - _DEF_CONST_ARR: $token$ addr = magic

    _TREE: rval of constant definition
        - _TREE_NODE: child, sibling
        - _TREE_LEAF: expr

    _STRING:  $token$

*/

class _STRING: public nodeAST {
public:
    string token;
    _STRING(string _token): token(_token) {}
    virtual string getToken() { return token; }
};

/* ==================================== */
/*                 _EXPR                */
/* ==================================== */
class _EXPR: public nodeAST {
public:
    _EXPR() {}
};
class _UNARY_OP: public _EXPR {
public:
    nodeAST *op;
    _UNARY_OP(nodeAST *_op): op(_op) {}
};
class _BINARY_OP: public _EXPR {
public:
    nodeAST *lop, *rop;
    _BINARY_OP(nodeAST *_lop, nodeAST *_rop): lop(_lop), rop(_rop) {}
};
class _NEG: public _UNARY_OP {
public:
    _NEG(nodeAST *_op): _UNARY_OP(_op) {}
    virtual int eval() { return - op->eval(); }
};
class _NOT: public _UNARY_OP {
public:
    _NOT(nodeAST *_op): _UNARY_OP(_op) {}
    virtual int eval() { return ! op->eval(); }
};
class _ADD: public _BINARY_OP {
public:
    _ADD(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() + rop->eval(); }
};
class _SUB: public _BINARY_OP {
public:
    _SUB(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() - rop->eval(); }
};
class _MUL: public _BINARY_OP {
public:
    _MUL(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() * rop->eval(); }
};
class _DIV: public _BINARY_OP {
public:
    _DIV(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() / rop->eval(); }
};
class _MOD: public _BINARY_OP {
public:
    _MOD(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() % rop->eval(); }
};
class _AND: public _BINARY_OP {
public:
    _AND(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() && rop->eval(); }
};
class _OR: public _BINARY_OP {
public:
    _OR(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() || rop->eval(); }
};
class _LT: public _BINARY_OP {
public:
    _LT(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() < rop->eval(); }
};
class _LE: public _BINARY_OP {
public:
    _LE(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() <= rop->eval(); }
};
class _GT: public _BINARY_OP {
public:
    _GT(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() > rop->eval(); }
};
class _GE: public _BINARY_OP {
public:
    _GE(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() >= rop->eval(); }
};
class _EQ: public _BINARY_OP {
public:
    _EQ(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() == rop->eval(); }
};
class _NEQ: public _BINARY_OP {
public:
    _NEQ(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() != rop->eval(); }
};
class _INTEGER: public _EXPR {
public:
    int num;
    _INTEGER(int _num): num(_num) {}
    virtual int eval() { return num; }
};
class _VAR: public _EXPR {
public:
    string token;
    _VAR(string _token): token(_token) {}
};
class _FUNC_CALL: public _EXPR {
public:
    string token; nodeAST *param;
    _FUNC_CALL(string _token, nodeAST *_param): token(_token), param(_param) {}
};
class _ARRAY_ITEM: public _EXPR {
public:
    string token; nodeAST *param;
    _ARRAY_ITEM(string _token, nodeAST *_param): token(_token), param(_param) {}
};


/* ==================================== */
/*                 _LIST                */
/* ==================================== */
class _LIST: public nodeAST {
public:
    nodeAST *head, *tail;
    _LIST(nodeAST *_head, nodeAST *_tail): head(_head), tail(_tail) {}
};
class _CALL_LIST: public _LIST {
public:
    _CALL_LIST(nodeAST *_head, nodeAST *_tail): _LIST(_head, _tail) {}
};
class _ADDR_LIST: public _LIST {
public:
    _ADDR_LIST(nodeAST *_head, nodeAST *_tail): _LIST(_head, _tail) {}
    virtual void vectorize(vector<int> &v) {
        if (head) v.push_back(head->eval());
        if (tail) tail->vectorize(v);
    }
};
class _STMT_SEQ: public _LIST {
public:
    _STMT_SEQ(nodeAST *_head, nodeAST *_tail): _LIST(_head, _tail) {}
};


/* ==================================== */
/*                 _STMT                */
/* ==================================== */
class _STMT: public nodeAST {
public:
    _STMT() {}
};
class _IF: public _STMT {
public:
    nodeAST *cond, *body;
    _IF(nodeAST *_cond, nodeAST *_body): cond(_cond), body(_body) {}
};
class _IF_THEN: public _STMT {
public:
    nodeAST *cond, *body, *ebody;
    _IF_THEN(nodeAST *_cond, nodeAST *_body, nodeAST *_ebody): cond(_cond), body(_body), ebody(_ebody) {}
};
class _WHILE: public _STMT {
public:
    nodeAST *cond, *body;
    _WHILE(nodeAST *_cond, nodeAST *_body): cond(_cond), body(_body) {}
};
class _RETURN_VOID: public _STMT {
public:
    _RETURN_VOID() {}
};
class _RETURN_EXPR: public _STMT {
public:
    nodeAST *expr;
    _RETURN_EXPR(nodeAST *_expr): expr(_expr) {}
};
class _CONTINUE: public _STMT {
public:
    _CONTINUE() {}
};


/* ==================================== */
/*                 _FUNC                */
/* ==================================== */
class _FUNC: public _STMT {
public:
    string token; nodeAST *param, *body;
    _FUNC(string _token, nodeAST *_param, nodeAST *_body): token(_token), param(_param), body(_body) {}
};

/* ==================================== */
/*                 _DECL                */
/* ==================================== */
class _DECL: public nodeAST {
public:
    _DECL() {}
};
class _DEF_VAR: public _DECL {
public:
    string token; nodeAST *expr;
    _DEF_VAR(string _token, nodeAST *_expr): token(_token), expr(_expr) {}
};
class _DEF_CONST_VAR: public _DEF_VAR {
public:
    _DEF_CONST_VAR(string _token, nodeAST *_expr): _DEF_VAR(_token, _expr) {}
};

class _DEF_ARR: public _DECL {
public:
    string token; nodeAST *addr, *expr;
    _DEF_ARR(string _token, nodeAST *_addr, nodeAST *_expr): token(_token), addr(_addr), expr(_expr) {}
};
class _DEF_CONST_ARR: public _DEF_ARR {
public:
    _DEF_CONST_ARR(string _token, nodeAST *_addr, nodeAST *_expr): _DEF_ARR(_token, _addr, _expr) {}
};

/* ==================================== */
/*                 _TREE                */
/* ==================================== */
class _TREE: public nodeAST {
public:
    _TREE *sibling;
    _TREE(): sibling(NULL) {}
    virtual bool leaf() { assert(false); }
    virtual nodeAST* getExpr() { assert(false); }
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
        for (int i = 1; i <= ws; i++) printf(" "); printf("-\n");
        child->debug(ws + 1);
        if (sibling) sibling->debug(ws);
    }
};
class _TREE_LEAF: public _TREE {
public:
    nodeAST *expr;
    _TREE_LEAF(nodeAST *_expr): _TREE(), expr(_expr) {}
    virtual bool leaf() { return true;}
    virtual nodeAST* getExpr() { return expr; }
    virtual void debug(int ws = 0) {
        for (int i = 1; i <= ws; i++) printf(" "); printf("unit\n");
        if (sibling) sibling->debug(ws);
    }
};