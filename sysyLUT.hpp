#include <bits/stdc++.h>
using namespace std;

class nodeAST;

class cellLUT {
public:
    string token;
    cellLUT(string _token): token(_token) {}
    virtual void instan() { assert(false); }
    virtual int eval() { assert(false); }
    virtual int eval( vector<int> &addr ) { assert(false); }
};

class cellVar: public cellLUT {
public:
    nodeAST *init;
    cellVar(string _token): cellLUT(_token) {}
};
class cellConstVar: public cellVar {
public:
    int init_const;
    cellConstVar(string _token): cellVar(_token) { init_const = 0; }
    virtual void instan() { if (init) init_const = init->eval(); }
    virtual int eval() { return init_const; }
};
class cellArr: public cellLUT {
public:
    vector<int> shape;
    vector<nodeAST *> inits;
    cellArr(string _token): cellLUT(_token) {}
};
class cellConstArr: public cellArr {
public:
    vector<int> inits_const;
    cellConstArr(string _token): cellArr(_token) {}
    virtual void instan() {
        int n = inits.size();
        inits_const.resize(n);
        for (int i = 0; i < n; i++)
            inits_const[i] = (inits[i] ? inits[i]->eval() : 0);
    }
    virtual int eval( vector<int> &addr ) {
        int dim = shape.size(), index = 0;
        for (int i = 0; i < dim; i++)
            index = index * shape[dim] + addr[dim];
        return inits_const[index];
    }
};
