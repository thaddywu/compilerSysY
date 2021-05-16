all: compiler
compiler: sysy.yy.cpp sysy.tab.cpp defs.hpp defs.cpp main.cpp \
	sysyLUT.cpp sysyAST.cpp sysyDATA.cpp \
	eeyoreAST.hpp eeyoreAST.cpp eeyoreREG.hpp tiggerAST.hpp 
	g++ -w -std=c++11 -DLOCALTEST -o compiler \
		sysy.yy.cpp sysy.tab.cpp sysyAST.cpp sysyLUT.cpp sysyDATA.cpp eeyoreAST.cpp defs.cpp main.cpp

sysy.yy.cpp: sysy.l sysy.tab.cpp
	flex -o sysy.yy.cpp sysy.l
sysy.tab.cpp: sysy.y sysyAST.hpp sysyLUT.hpp sysyDATA.hpp
	bison -d -Wconflicts-sr -Wconflicts-rr -o sysy.tab.cpp sysy.y
clean:
	rm sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler