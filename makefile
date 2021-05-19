all: compiler
compiler: sysy.yy.cpp sysy.tab.cpp defs.hpp defs.cpp main.cpp \
	sysyLUT.cpp sysyAST.cpp sysyDATA.cpp \
	eeyoreAST.hpp eeyoreAST.cpp eeyoreREG.hpp eeyoreOPT.cpp tiggerAST.hpp 
	g++ -w -std=c++11 -DLOCALTEST -o compiler \
		sysy.yy.cpp sysy.tab.cpp sysyAST.cpp sysyLUT.cpp sysyDATA.cpp eeyoreAST.cpp eeyoreOPT.cpp defs.cpp main.cpp

sysy.yy.cpp: sysy.l sysy.tab.cpp
	flex -o sysy.yy.cpp sysy.l
sysy.tab.cpp: sysy.y sysyAST.hpp sysyLUT.hpp sysyDATA.hpp
	bison -d -Wconflicts-sr -Wconflicts-rr -o sysy.tab.cpp sysy.y
clean:
	rm sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler
run:
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output
git:
	rm *.eeyore *.tigger *.sy output output.S
	rm sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler
	rm *.out *.in *.ans