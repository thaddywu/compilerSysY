#include <bits/stdc++.h>
using namespace std;

#define Reg_s 8
#define Reg_t 7
#define Reg_a 8
#define Reg_s_all (Reg_s + 4)
#define Reg_N (Reg_s + Reg_t + Reg_a)

#define reserved_reg1 "s9"
#define reserved_reg2 "s10"
#define reserved_reg3 "s11"

extern string t0 ;

extern void print(string x) ;
extern void printTab(string x) ;
extern bool isreg(string x) ;
extern bool islogicop(string x) ;
extern bool isint10(int x) ;
extern bool isint12(int x) ;

extern int STK ;

class tiggerAST {
public:
    tiggerAST() {}
    ~tiggerAST() {}
    virtual void Dump() { assert(false); } // printCode
    virtual void translate() { assert(false); } // print Risv-V code
};

class _tGLBVAR: public tiggerAST {
public:
    string var;
    _tGLBVAR(string _var): var(_var) {}
    virtual void Dump() { print(var + " = 0"); }
    virtual void translate() {
        printTab(".global " + var);
        printTab(".section .sdata");
        printTab(".align 2");
        printTab(".type " + var + ", @object");
        printTab(".size " + var + ", 4");
        print(var + ":");
        printTab(".word 0");
    }
};
class _tGLBARR: public tiggerAST {
public:
    string var; int size; // without * 4
    _tGLBARR(string _var, int _size): var(_var), size(_size) {}
    virtual void Dump() { print(var + " = malloc " + to_string(size * 4)); }
    virtual void translate() {
        printTab(".comm " + var + ", " + to_string(size * 4) + ", 4");
    }
};
class _tFUNC: public tiggerAST {
public:
    tiggerAST *body; string func; int arity, mem; // mem without * 4
    _tFUNC(string _func, int _arity): func(_func), arity(_arity) { mem = 0; }
    _tFUNC(string _func, int _arity, int _mem, tiggerAST *_body): func(_func), arity(_arity), mem(_mem), body(_body) {}
    virtual void Dump() { print("f_" + func + " [" + to_string(arity) + "] [" + to_string(mem) + "]"); body->Dump(); print("end f_" + func); }
    virtual void translate() {
        STK = (mem / 4 + 1) * 16;
        printTab(".text");
        printTab(".align 2");
        printTab(".global " + func);
        printTab(".type " + func + ", @function");
        print(func + ":");
        if (isint12(-STK))
        /* warning: make sure -STK is int12 */
            printTab("addi sp, sp, " + to_string(-STK));
        else {
            printTab("li " + t0 + ", " + to_string(-STK));
            printTab("add sp, sp, " + t0);
        }
        if (isint12(STK-4))
            printTab("sw ra, " + to_string(STK-4) + "(sp)");
        else {
            printTab("li " + t0 + ", " + to_string(STK-4));
            printTab("add " + t0 + ", " + t0 + ", sp");
            printTab("sw ra, 0(" + t0 + ")");
        }

        body->translate();

        printTab(".size " + func + ", .-" + func);
    }
};
class _tSEQ: public tiggerAST {
public:
    vector<tiggerAST *> seq;
    _tSEQ(vector<tiggerAST *> _seq): seq(_seq) {}
    virtual void Dump() { for (auto stmt: seq) stmt->Dump(); }
    virtual void translate() { for (auto stmt: seq) stmt->translate(); }
};
class _tDIRECT: public tiggerAST {
public:
    string d, s;
    _tDIRECT(string _d, int _s): d(_d), s(to_string(_s)) { assert(isreg(d)); }
    _tDIRECT(string _d, string _s): d(_d), s(_s) { assert(isreg(d)); }
    virtual void Dump() { printTab(d + " = " + s); }
    virtual void translate() {
        if (d == s) return ;
        if (isreg(s))
            printTab("mv " + d + ", " + s);
        else
            printTab("li " + d + ", " + s);
    }
};
class _tUNARY: public tiggerAST {
public:
    string d, op, s;
    _tUNARY(string _d, string _op, string _s): d(_d), op(_op), s(_s) { assert(isreg(d) && isreg(s) && (op == "-" || op == "!")); }
    virtual void Dump() { printTab(d + " = " + op + s); }
    virtual void translate() {
        if (op == "-")
            printTab("neg " + d + ", " + s);
        if (op == "!")
            printTab("seqz " + d + ", " + s);
    }
};
class _tBINARY: public tiggerAST {
public:
    string d, s, op, t; int t_int;
    _tBINARY(string _d, string _s, string _op, string _t): d(_d), op(_op), s(_s), t(_t) { assert(isreg(d) && isreg(s)); }
    _tBINARY(string _d, string _s, string _op, int _t): d(_d), op(_op), s(_s), t(to_string(_t)), t_int(_t) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + " = " + s + " " + op + " " + t); }
    virtual void translate() {
        if (!isreg(t)) {
            if (op == "*" && t_int == 2) 
                { printTab("slli " + d + ", " + s + ", 1"); return ; }
            if (op == "*" && t_int == 4) 
                { printTab("slli " + d + ", " + s + ", 2"); return ; }
            if (op == "/" && t_int == 2) 
                { printTab("srai " + d + ", " + s + ", 1"); return ; }
            if (op == "\%" && t_int == 2) 
                { printTab("andi " + d + ", " + s + ", 1"); return ; }
            if ((op == "+" || op == "-") && t_int == 0) 
                { if (d != s) printTab("mv " + d + ", " + s); return ; }
            if (op == "*" && t_int == 0) 
                { printTab("li " + d + ", 0"); return ; }
    
        }
        if (!isreg(t) && isint12(t_int) && (op == "+" || op == "<")) {
            /* t:int12, op: + < */
            if (op == "+")
                printTab("addi " + d + ", " + s + ", " + t);
            if (op == "<")
                printTab("slti " + d + ", " + s + ", " + t);
        }
        else {
            if (!isreg(t)) {
                printTab("li " + t0 + ", " + t);
                t = t0;
            }
            if (op == "+")
                printTab("add " + d + ", " + s + ", " + t);
            if (op == "-")
                printTab("sub " + d + ", " + s + ", " + t);
            if (op == "*")
                printTab("mul " + d + ", " + s + ", " + t);
            if (op == "/")
                printTab("div " + d + ", " + s + ", " + t);
            if (op == "\%")
                printTab("rem " + d + ", " + s + ", " + t);
            if (op == "<")
                printTab("slt " + d + ", " + s + ", " + t);
            if (op == ">")
                printTab("sgt " + d + ", " + s + ", " + t);
            if (op == "<=") {
                printTab("sgt " + d + ", " + s + ", " + t);
                printTab("seqz " + d + ", " + d);
            }
            if (op == ">=") {
                printTab("slt " + d + ", " + s + ", " + t);
                printTab("seqz " + d + ", " + d);
            }
            if (op == "&&") {
                printTab("snez " + d + ", " + s);
                printTab("snez " + t0 + ", " + t);
                printTab("and " + d + ", " + d + ", " + t0);
            }
            if (op == "||") {
                printTab("or " + d + ", " + s + ", " + t);
                printTab("snez " + d + ", " + d);
            }
            if (op == "!=") {
                printTab("xor " + d + ", " + s + ", " + t);
                printTab("snez " + d + ", " + d);
            }
            if (op == "==") {
                printTab("xor " + d + ", " + s + ", " + t);
                printTab("seqz " + d + ", " + d);
            }
        }
    }
};
class _tSAVE: public tiggerAST {
public:
    string d, x, s; int x_int;
    _tSAVE(string _d, int _x, string _s): d(_d), x(to_string(_x)), x_int(_x), s(_s) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + "[" + x + "] = " + s); }
    virtual void translate() {
        if (isint12(x_int))
            printTab("sw " + s + ", " + x + "(" + d + ")");
        else {
            printTab("li " + t0 + ", " + x);
            printTab("add " + t0 + ", " + t0 + ", " + d);
            printTab("sw " + s + ", 0(" + t0 + ")");
        }
    }
};
class _tSEEK: public tiggerAST {
public:
    string d, s, x; int x_int;
    _tSEEK(string _d, string _s, int _x): d(_d), s(_s), x(to_string(_x)), x_int(_x) { assert(isreg(d) && isreg(s)); }
    virtual void Dump() { printTab(d + " = " + s + "[" + x + "]"); }
    virtual void translate() {
        if (isint12(x_int))
            printTab("lw " + d + ", " + x + "(" + s + ")");
        else {
            printTab("li " + t0 + ", " + x);
            printTab("add " + t0 + ", " + t0 + ", " + s);
            printTab("lw " + d + ", 0(" + t0 + ")");
        }
    }
};
class _tIFGOTO: public tiggerAST {
public:
    string s, op, t, l;
    _tIFGOTO(string _s, string _op, string _t, string _l): s(_s), op(_op), t(_t), l(_l) { assert(isreg(s) && isreg(t) && islogicop(op)); }
    virtual void Dump() { printTab("if " + s + " " + op + " " + t + " goto " + l); }
    virtual void translate() {
        if (op == "<")
            printTab("blt " + s + ", " + t + ", ." + l);
        if (op == ">")
            printTab("bgt " + s + ", " + t + ", ." + l);
        if (op == "<=")
            printTab("ble " + s + ", " + t + ", ." + l);
        if (op == ">=")
            printTab("bge " + s + ", " + t + ", ." + l);
        if (op == "!=")
            printTab("bne " + s + ", " + t + ", ." + l);
        if (op == "==")
            printTab("beq " + s + ", " + t + ", ." + l);
    }
};
class _tGOTO: public tiggerAST {
public:
    string l;
    _tGOTO(string _l): l(_l) {}
    virtual void Dump() { printTab("goto " + l); }
    virtual void translate() { printTab("j ." + l); }
};
class _tLABEL: public tiggerAST {
public:
    string l;
    _tLABEL(string _l): l(_l) {}
    virtual void Dump() { print(l + ":"); }
    virtual void translate() { print("." + l + ":"); }
};
class _tCALL: public tiggerAST {
public:
    string func;
    _tCALL(string _func): func(_func) {}
    virtual void Dump() { printTab("call f_" + func); }
    virtual void translate() { printTab("call " + func); }
};
class _tRETURN: public tiggerAST {
public:
    _tRETURN() {}
    virtual void Dump() { printTab("return"); }
    virtual void translate() {
        if (isint12(STK-4))
            printTab("lw ra, " + to_string(STK-4) + "(sp)");
        else {
            printTab("li " + t0 + ", " + to_string(STK-4));
            printTab("add " + t0 + ", " + t0 + ", sp");
            printTab("lw ra, 0(" + t0 + ")");
        }
        if (isint12(STK))
            printTab("addi sp, sp, " + to_string(STK));
        else {
            printTab("li " + t0 + ", " + to_string(STK));
            printTab("add sp, sp, " + t0);
        }
        printTab("ret");
    }
};
class _tLOAD: public tiggerAST {
public:
    string s, reg; int s_int; bool global_var;
    _tLOAD(string _s, string _reg): s(_s), reg(_reg) { global_var = true; assert(s[0] == 'v' && !isreg(s) && isreg(reg)); }
    _tLOAD(int _s, string _reg): s_int(_s), s(to_string(_s)), reg(_reg) { global_var = false; assert(isreg(reg)); }
    virtual void Dump() { printTab("load " + s + " " + reg); }
    virtual void translate() {
        if (global_var) {
            printTab("lui " + reg + ", %hi(" + s + ")");
            printTab("lw " + reg + ", %lo(" + s + ")(" + reg + ")");
        }
        else {
            if (isint10(s_int))
                printTab("lw " + reg + ", " + to_string(s_int*4) + "(sp)");
            else {
                printTab("li " + t0 + ", " + to_string(s_int*4));
                printTab("add " + t0 + ", " + t0 + ", sp");
                printTab("lw " + reg + ", 0(" + t0 + ")");
            }
        }
    }
};
class _tLOADADDR: public tiggerAST {
public:
    string s, reg; int s_int; bool global_var;
    _tLOADADDR(string _s, string _reg): s(_s), reg(_reg) { global_var = true; assert(s[0] == 'v' && !isreg(s) && isreg(reg)); }
    _tLOADADDR(int _s, string _reg): s_int(_s), s(to_string(_s)), reg(_reg) { global_var = false; assert(!isreg(s) && isreg(reg)); }
    virtual void Dump() { printTab("loadaddr " + s + " " + reg); }
    virtual void translate() {
        if (global_var)
            printTab("la " + reg + ", " + s);
        else {
            if (isint10(s_int))
                printTab("addi " + reg + ", sp, " + to_string(s_int*4));
            else {
                printTab("li " + t0 + ", " + to_string(s_int*4));
                printTab("add " + reg + ", sp, " + t0);
            }
        }
    }
};
class _tSTORE: public tiggerAST {
public:
    string reg; int s_int;
    _tSTORE(string _reg, int _s): reg(_reg), s_int(_s) { assert(isreg(reg)); }
    virtual void Dump() { printTab("store " + reg + " " + to_string(s_int)); }
    virtual void translate() {
        if (isint10(s_int))
            printTab("sw " + reg + ", " + to_string(s_int*4) + "(sp)");
        else {
            printTab("li " + t0 + ", " + to_string(s_int*4));
            printTab("add " + t0 + ", " + t0 + ", sp");
            printTab("sw " + reg + ", 0(" + t0 + ")");
        }
    }
};