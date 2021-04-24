all: compiler
compiler: sysy.yy.cpp sysy.tab.cpp sysyAST.hpp sysyLUT.hpp
	g++ -w -std=c++11 -o compiler sysy.yy.cpp sysy.tab.cpp
sysy.yy.cpp: sysy.l sysy.y
	flex -o sysy.yy.cpp sysy.l
sysy.tab.cpp: sysy.y
	bison -d -o sysy.tab.cpp sysy.y
clean:
	rm sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler