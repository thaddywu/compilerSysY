#include <bits/stdc++.h>
using namespace std;

class cellLUT {
public:
    string token, etoken; /* etoken : token in eeyore */
    cellLUT(string _token): token(_token) {}
    virtual void instan() { assert(false); }
    virtual int eval() { assert(false); }
    virtual int eval( vector<int> &addr ) { assert(false); }
    virtual void debug() { assert(false); }
    virtual void parse() { assert(false); }
    void setEToken(string _etoken) { etoken = _etoken; }
    string getEToken() { return etoken; }
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
    virtual void parse() {

    }
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

class TokenManager {
private:
    map<string, cellLUT*> table;
    typedef pair<cellLUT*, cellLUT*> cellPair;
    typedef vector<cellPair> Record;
    stack< Record > record;
    int tempNum, globalNum, cpNum; /* #t, #T, #l*/
    
public:
    TokenManager(): tempNum(0), globalNum(0) { assert(record.empty()); }
    void descend() { record.push({});}
    void insert(cellLUT* token) {
        if (table.find(token->token) != table.end()) {
            cellLUT* old = table[token->token];
            record.top().push_back( cellPair(old, token));
        }
        else {
            record.top().push_back( cellPair(NULL, token));
        }
        table[token->token] = token;
    }
    void ascend() {
        for (auto item: record.top())
            table[item.second->token] = item.first;
        record.pop();
    }
    int newt() { return tempNum++; }
    int newT() { return globalNum++; }
    int newl() { return cpNum++; }
    
};
TokenManager *tokenManager;