all: compiler
compiler: sysy.yy.cpp sysy.tab.cpp defs.hpp main.cpp eeyoreAST.hpp
	g++ -w -std=c++11 -DLOCALTEST -o compiler sysy.yy.cpp sysy.tab.cpp sysyAST.cpp defs.cpp main.cpp
sysy.yy.cpp: sysy.l sysy.tab.cpp sysyAST.hpp sysyAST.cpp sysyLUT.hpp
	flex -o sysy.yy.cpp sysy.l
sysy.tab.cpp: sysy.y sysyAST.hpp sysyAST.cpp sysyLUT.hpp
	bison -d -Wconflicts-sr -Wconflicts-rr -o sysy.tab.cpp sysy.y
clean:
	rm sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler