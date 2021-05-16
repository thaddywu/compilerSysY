#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

extern void yyparse();

#ifdef LOCALTEST
int main(int argc, char **argv) {
    string file_name = argv[1];
    if (file_name[-3 + file_name.size()] == '.')
        file_name = file_name.substr(0, -3 + file_name.size());
    string sysy_file = file_name + ".sy";
    string eeyore_file = file_name + ".eeyore";
    string tigger_file = file_name + ".tigger";
    string riscv_file = file_name + ".asm";
    freopen(sysy_file.c_str(), "r", stdin);
#else
int main(int argc, char **argv)
{
    if (argc == 6) {
        freopen(argv[3], "r", stdin);
        freopen(argv[5], "w", stdout);
    }
    else {
        freopen(argv[2], "r", stdin);
        freopen(argv[4], "w", stdout);
    }
#endif
    varManager->newEnviron();
    yyparse();
    sysyRoot->translate("", "", true);
#ifdef LOCALTEST
    freopen(eeyore_file.c_str(), "w", stdout);
    eeyoreRoot->Dump();
    freopen(tigger_file.c_str(), "w", stdout);
    eeyoreRoot->translate();
    tiggerRoot->Dump();
    freopen(riscv_file.c_str(), "w", stdout);
    tiggerRoot->translate();
#else
    if (strcmp(argv[2], "-e") == 0)
        eeyoreRoot->Dump();
        /* compile to eeyore */
    else {
        eeyoreRoot->translate();
        if (strcmp(argv[2], "-t") == 0)
            tiggerRoot->Dump();
        else
            tiggerRoot->translate();
    }
#endif
    return 0;
}