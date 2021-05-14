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
    freopen(argv[3], "r", stdin);
    freopen(argv[5], "w", stdout);
#endif
    tokenManager->newEnviron();
    yyparse();
    sysyRoot->translate("", "", true);
#ifndef LOCALTEST
    if (strcmp(argv[2], "-e") == 0)
        eeyoreRoot->Dump();
        /* compile to eeyore */
    else {
#endif
        eeyoreRoot->translate();
        tiggerRoot->Dump();
#ifndef LOCALTEST
    }
#endif
    return 0;
}