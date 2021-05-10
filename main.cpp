#ifndef defs_hpp
#include "defs.hpp"
#define defs_hpp
#endif
using namespace std;

extern void yyparse();

#ifdef LOCALTEST
int main() {
#else
int main(int argc, char **argv)
{
    assert(argc == 6);
    assert(strcmp(argv[1], "-S") == 0);
    assert(strcmp(argv[2], "-e") == 0);
    assert(strcmp(argv[4], "-o") == 0);

    freopen(argv[3], "r", stdin);
    freopen(argv[5], "w", stdout);
#endif
    tokenManager->newEnviron();
    yyparse();
    sysyRoot->translate("", "", true);
    eeyoreRoot->Dump();
    return 0;
}