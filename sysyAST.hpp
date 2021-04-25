#include <bits/stdc++.h>
using namespace std;

extern void printDecl(string str) ;
extern void bufferStmt(string str) ;
extern void printStmt() ;

class nodeAST {
public:
    nodeAST() {}
    ~nodeAST() {}
        // _STR:: return string
    virtual string getToken() { assert(false); }
        // _EXPR:: eval expression, reduce to var & return operational symbol
    virtual int eval() { assert(false); }
    virtual string atomize() { assert(false); }
    virtual string atomize(string token) { assert(false); }
    virtual string symbol() { assert(false); }
    virtual string lvalize() { assert(false); }
        // _ADDR_LIST:: vectorize list & reduce to unary var
    virtual void vectorize(vector<int> &v) { assert(false); }
        // _CALL_LIST:: param pass
    virtual void pass() { assert(false); }
        // _DECL:
    virtual void instantialize() { assert(false); }
    virtual void initialize() { assert(false); }
        // basic function for all node

    virtual int countp() { assert(false); }

    virtual void traverse(string cp, bool glb) { assert(false); }
    /* For every function, decl first, then traverse */
    /* Logic structrue:
        global : decl , (stop at function)
                then traverse
        func : print structure, then decl, then traverse
                in main, implement initialization of global vars
    */

    /* some virtual functions are embedded in _TREE,
        so as to avoid enormous declaration in nodeAST,
        but therefore explicit conversion is needed,
        when using functions of _TREE
    */
};

/*
    Definition of nodes, so as to construct Abstraction Syntax Tree
    _PROGRAM:
        whole structure
    _EXPR:
        - _BINARY_OP: lop, rop
            - _AND, _SUB, _DIV, _MUL, _MOD, _AND, _OR, _LT, _GT, _LE, _GE, _EQ, _NEQ
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
                - _PARAM_VAR: in function declaration
            - _DEF_ARR: $token$ addr = magic
                - _DEF_CONST_ARR: $token$ addr = magic
                - _PARAM_ARR: in function declaration
        - _BLOCK: { .. }

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

class _PROGRAM: public nodeAST {
public:
    nodeAST *program;
    _PROGRAM(nodeAST *_program): program(_program) {}
    virtual void traverse(string cp, bool glb) ;
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
    virtual string atomize() ;
};
class _BINARY_OP: public _EXPR {
public:
    nodeAST *lop, *rop;
    _BINARY_OP(nodeAST *_lop, nodeAST *_rop): lop(_lop), rop(_rop) {}
    virtual string atomize() ;
};
class _NEG: public _UNARY_OP {
public:
    _NEG(nodeAST *_op): _UNARY_OP(_op) {}
    virtual int eval() { return - op->eval(); }
    virtual string symbol() { return "-"; }
};
class _NOT: public _UNARY_OP {
public:
    _NOT(nodeAST *_op): _UNARY_OP(_op) {}
    virtual int eval() { return ! op->eval(); }
    virtual string symbol() { return "!"; }
};
class _ADD: public _BINARY_OP {
public:
    _ADD(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() + rop->eval(); }
    virtual string symbol() { return "+"; }
};
class _SUB: public _BINARY_OP {
public:
    _SUB(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() - rop->eval(); }
    virtual string symbol() { return "-"; }
};
class _MUL: public _BINARY_OP {
public:
    _MUL(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() * rop->eval(); }
    virtual string symbol() { return "*"; }
};
class _DIV: public _BINARY_OP {
public:
    _DIV(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() / rop->eval(); }
    virtual string symbol() { return "/"; }
};
class _MOD: public _BINARY_OP {
public:
    _MOD(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() % rop->eval(); }
    virtual string symbol() { return "\%"; }
};
class _AND: public _BINARY_OP {
public:
    _AND(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() && rop->eval(); }
    virtual string symbol() { return "&&"; }
};
class _OR: public _BINARY_OP {
public:
    _OR(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() || rop->eval(); }
    virtual string symbol() { return "||"; }
};
class _LT: public _BINARY_OP {
public:
    _LT(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() < rop->eval(); }
    virtual string symbol() { return "<"; }
};
class _LE: public _BINARY_OP {
public:
    _LE(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() <= rop->eval(); }
    virtual string symbol() { return "<="; }
};
class _GT: public _BINARY_OP {
public:
    _GT(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() > rop->eval(); }
    virtual string symbol() { return ">"; }
};
class _GE: public _BINARY_OP {
public:
    _GE(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() >= rop->eval(); }
    virtual string symbol() { return ">="; }
};
class _EQ: public _BINARY_OP {
public:
    _EQ(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() == rop->eval(); }
    virtual string symbol() { return "=="; }
};
class _NEQ: public _BINARY_OP {
public:
    _NEQ(nodeAST *_lop, nodeAST *_rop): _BINARY_OP(_lop, _rop) {}
    virtual int eval() { return lop->eval() != rop->eval(); }
    virtual string symbol() { return "!="; }
};
class _INTEGER: public _EXPR {
public:
    int num;
    _INTEGER(int _num): num(_num) {}
    virtual int eval() { return num; }
    virtual string atomize() { return to_string(num); }
};
class _VAR: public _EXPR {
public:
    string token;
    _VAR(string _token): token(_token) {}
    virtual int eval();
    virtual string atomize() ;
    virtual string lvalize() ;
};
class _FUNC_CALL: public _EXPR {
public:
    string token; nodeAST *param;
    _FUNC_CALL(string _token, nodeAST *_param): token(_token), param(_param) {}
    virtual string atomize() ;
    virtual void traverse(string cp, bool glb);
};
class _ARRAY_ITEM: public _EXPR {
public:
    string token; nodeAST *param;
    _ARRAY_ITEM(string _token, nodeAST *_param): token(_token), param(_param) {}
    virtual int eval();
    virtual string atomize() ;
    virtual string lvalize() ;
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
    virtual void pass() ;
    virtual string atomize() { assert(false); }
};
class _ADDR_LIST: public _LIST {
public:
    _ADDR_LIST(nodeAST *_head, nodeAST *_tail): _LIST(_head, _tail) {}
    virtual void vectorize(vector<int> &v) {
        if (head) v.push_back(head->eval());
        if (tail) tail->vectorize(v);
    }
    virtual string atomize(string token) ;
};
class _PARAM_LIST: public _LIST {
public:
    _PARAM_LIST(nodeAST *_head, nodeAST *_tail): _LIST(_head, _tail) {}
    virtual int countp() { return (head ? 1 : 0) + (tail ? tail->countp() : 0);}
    virtual void traverse(string cp, bool glb);
};
class _STMT_SEQ: public _LIST {
public:
    _STMT_SEQ(nodeAST *_head, nodeAST *_tail): _LIST(_head, _tail) {}
    virtual void traverse(string cp, bool glb);
    virtual int countp() { return (head ? head->countp() : 0) + (tail ? tail->countp() : 0);}
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
    virtual void traverse(string cp, bool glb);
};
class _IF_ELSE: public _STMT {
public:
    nodeAST *cond, *body, *ebody;
    _IF_ELSE(nodeAST *_cond, nodeAST *_body, nodeAST *_ebody): cond(_cond), body(_body), ebody(_ebody) {}
    virtual void traverse(string cp, bool glb);
};
class _WHILE: public _STMT {
public:
    nodeAST *cond, *body;
    _WHILE(nodeAST *_cond, nodeAST *_body): cond(_cond), body(_body) {}
    virtual void traverse(string cp, bool glb);
};
class _RETURN_VOID: public _STMT {
public:
    _RETURN_VOID() {}
    virtual void traverse(string cp, bool glb);
};
class _RETURN_EXPR: public _STMT {
public:
    nodeAST *expr;
    _RETURN_EXPR(nodeAST *_expr): expr(_expr) {}
    virtual void traverse(string cp, bool glb);
};
class _CONTINUE: public _STMT {
public:
    _CONTINUE() {}
    virtual void traverse(string cp, bool glb);
};
class _ASSIGN: public _STMT {
public:
    nodeAST *lop, *rop;
    _ASSIGN(nodeAST *_lop, nodeAST *_rop): lop(_lop), rop(_rop) {}
    virtual void traverse(string cp, bool glb);
};
class _BLOCK: public _STMT {
public:
    nodeAST *block;
    _BLOCK(nodeAST *_block): block(_block) {}
    virtual void traverse(string cp, bool glb);
};


/* ==================================== */
/*                 _FUNC                */
/* ==================================== */
class _FUNC: public _STMT {
public:
    string token; nodeAST *param, *body;
    _FUNC(string _token, nodeAST *_param, nodeAST *_body): token(_token), param(_param), body(_body) {}
    virtual void traverse(string cp, bool glb);
};

/* ==================================== */
/*                 _DECL                */
/* ==================================== */
class _DECL: public nodeAST {
public:
    _DECL() {}
    virtual void instantialize() {} /* do nothing except for CONST */
};
class _DEF_VAR: public _DECL {
public:
    string token; nodeAST *inits;
    _DEF_VAR(string _token, nodeAST *_inits): token(_token), inits(_inits) {}
    virtual void traverse(string cp, bool glb);
    virtual void initialize() ;
};
class _DEF_CONST_VAR: public _DEF_VAR {
public:
    _DEF_CONST_VAR(string _token, nodeAST *_inits): _DEF_VAR(_token, _inits) {}
    virtual void instantialize();
};
class _PARAM_VAR: public _DEF_VAR {
public:
    _PARAM_VAR(string _token, nodeAST *_inits): _DEF_VAR(_token, _inits) {}
    virtual void traverse(string cp, bool glb);
    virtual void initialize() { assert(false); } /* redundant assertion */
};

class _DEF_ARR: public _DECL {
public:
    string token; nodeAST *addr, *inits;
    _DEF_ARR(string _token, nodeAST *_addr, nodeAST *_inits): token(_token), addr(_addr), inits(_inits) {}
    virtual void traverse(string cp, bool glb);
    virtual void initialize() ;
};
class _DEF_CONST_ARR: public _DEF_ARR {
public:
    _DEF_CONST_ARR(string _token, nodeAST *_addr, nodeAST *_inits): _DEF_ARR(_token, _addr, _inits) {}
    virtual void instantialize();
};
class _PARAM_ARR: public _DEF_ARR {
public:
    _PARAM_ARR(string _token, nodeAST *_addr, nodeAST *_inits): _DEF_ARR(_token, _addr, _inits) {}
    virtual void traverse(string cp, bool glb);
    virtual void initialize() { assert(false); } /* redundant assertion */
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
    virtual bool leaf() { return true; }
    virtual nodeAST* getExpr() { return expr; }
    virtual void debug(int ws = 0) {
        for (int i = 1; i <= ws; i++) printf(" "); printf("unit\n");
        if (sibling) sibling->debug(ws);
    }
};