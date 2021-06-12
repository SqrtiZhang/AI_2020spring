### 运行环境

Windows，需支持c11

如果需要在Linux跑，需要将`#include <windows.h>`去掉，并将计时函数改成`clock()`



### 编译

`g++ -o IDA AI_lab1_1_IDA.cpp`



### 运行命令行参数

#### Digit

第一个参数：输入原始文件地址

第二个参数: 输入目的状态文件的地址

第三个参数：7bolck在启发函数时候所乘的权值(double)

第四个参数：计算耗散值时启发式函数乘的权值(double)

第五个参数：输出文件地址

`IDA.exe input1.txt 3 1 output1.txt`

#### Sudoku

第一个参数：原始状态输入文件

第二个参数：输出文件

第三个参数：是否使用启发式函数等优化(1为使用，其他输入认为不使用)

`sudoku.exe sudoku01.txt result.txt 1`

