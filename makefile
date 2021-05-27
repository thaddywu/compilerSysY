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
fft:
	cp ../test-case/fft0.sy fft0.sy
	./compiler fft0
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output <../test-case/fft0.in >../test-case/fft0.out
	diff ../test-case/fft0.out ../test-case/fft0.ans
mm:
	cp ../test-case/01_mm1.sy 01_mm1.sy
	./compiler 01_mm1
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output <../test-case/01_mm1.in >../test-case/01_mm1.out
	diff ../test-case/01_mm1.out ../test-case/01_mm1.ans
bitset:
	cp ../test-case/00_bitset1.sy 00_bitset1.sy
	./compiler 00_bitset1
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output <../test-case/00_bitset1.in >../test-case/00_bitset1.out
	diff ../test-case/00_bitset1.out ../test-case/00_bitset1.ans
conv:
	cp ../test-case/conv1.sy conv1.sy
	./compiler conv1
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output <../test-case/conv1.in >../test-case/conv1.out
	diff ../test-case/conv1.out ../test-case/conv1.ans
transpose:
	cp ../test-case/transpose0.sy transpose0.sy
	./compiler transpose0
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output <../test-case/transpose0.in >../test-case/transpose0.out
	diff ../test-case/transpose0.out ../test-case/transpose0.ans
spmv:
	cp ../test-case/04_spmv1.sy 04_spmv1.sy
	./compiler 04_spmv1
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output <../test-case/04_spmv1.in >../test-case/04_spmv1.out
	diff ../test-case/04_spmv1.out ../test-case/04_spmv1.ans
run:
	riscv32-unknown-linux-gnu-gcc output.S -o output -L/root -lsysy -static
	qemu-riscv32-static output
git:
	rm *.eeyore *.tigger *.sy output output.S \
	sysy.tab.hpp sysy.tab.cpp sysy.yy.cpp compiler \
	*.out *.in *.ans
gitlab:
	echo https://test:TsZ_quoj3iNzX5S2asrq@gitlab.com/thaddywu/compilerSysY.git
github:
	echo https://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@github.com/thaddywu/compilerSysY.git
connect:
	sudo docker attach $(sudo docker run -dit -P -v /compiler:/compiler riscv-dev-env)
