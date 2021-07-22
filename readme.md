## compilerSysY
  > 该项目是一个编译2021春季的大作业，实现了简单的 sysY 编译器。
  
  性能测试跑的快主要归功于 bit-prob.cpp 里的手写汇编，其他优化都是最基本的，没什么用。
  
  未优化前的版本在 [fc4598c](https://github.com/thaddywu/compilerSysY/tree/fc4598cdfce8f8c0f51ad486caf8831a22862903)
  
## File Structure
  - sysy.l & sysy.y 前端。
  - *AST.cpp/hpp 分别对应三阶段的 AST 结构，翻译和导出代码的工作都实现在对应文件里。
  - eeyoreOPT.cpp Eeyore 阶段的优化部分。
  - eeyoreREG.hpp 完成寄存器分配工作。
  - sysyLUT.cpp/hpp SysY 的符号表，主要是用来处理作用域嵌套时的变量重名问题。
  - sysyDATA.cpp/hpp 处理多维数组和初始化。
