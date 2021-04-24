all: compiler
compiler: sysy.yy.cpp sysy.tab.cpp
	g++ -w -std=c++11 -o compiler sysy.yy.cpp sysy.tab.cpp
sysy.yy.cpp: sysy.l sysy.tab.cpp
	flex -o sysy.yy.cpp sysy.l
sysy.tab.cpp: sysy.y sysyAST.hpp sysyLUT.hpp
	bison -d -o sysy.tab.cpp sysy.y
clean:
	rm sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler