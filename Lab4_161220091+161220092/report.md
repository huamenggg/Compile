# 编译原理实验四实验报告
161220091 马可欣 3476799672@qq.com  
161220092 孟华 161220092@smail.nju.edu.cn

# 一、实现功能
在词法分析、语法分析、语义分析和中间代码生成程序的基础上，将C—源代码翻译为MIPS32指令序列，并在SPIM Simulator上运行。

# 二、如何编译
* 编译：make
* 运行：./parser test out.s
* 测试：qtspim

# 三、实验数据结构
我们在实验三的基础上实现了数据结构，在写入文件时对生成的中间代码进行了翻译。
* 添加了寄存器结构体
```javascript
struct Reg_ {
	Operand op;
	enum { AVAILABLE, LOCKED } status;//判断寄存器是否可用
	char name[4];
};
```
* 在InterCode结构体中添加了状态属性，判断语句是否被翻译完毕
```javascript
struct InterCode_ {
	enum InterCodeKind kind;
	union {
		struct { Operand result, op1, op2; } binop;
		struct { Operand right, left; } assign;
		struct { Operand t1, op, t2, label; } cond1;
		struct { Operand func, place; } call;
		struct { Operand op; int size; } dec;
		Operand value;
	} u;
	enum {LIVE, DEAD} status;//实验四新增
};
```

# 四、问题及思考
* 测试教程pdf中的测试用例一时，我们发现，根据原来的实现，while中判断语句的中间代码是最先翻译的。比如while(i<n)中比较i和n时，会比较0($sp)与4($sp)的大小，但是如果在循环中定义新的变量，新的变量被压栈，原来的i和n就会改变在栈里的位置，此时通过判断0($sp)与4($sp)进行跳转就会出错。针对这个问题，我们在WHILE语句的中间代码前插入自定义的中间代码，写入文件时判断到这条中间代码时向后扫描，如果while循环中存在定义语句，则提前翻译，并将该语句设为已翻译(DEAD)，因此新的变量会在i和n前被压栈，而不会改变它们在栈里的位置。














