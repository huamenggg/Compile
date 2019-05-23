## 编译原理实验三实验报告

161220092 孟华 161220092@smail.nju.edu.cn

161220091 马可欣 

### 一、实验数据结构

因为完成实验二的时候，轻信了一句话“语法树之后都没有用了”， 所以将语法树所有的相关实现都删掉了，事实证明.....orz

好的， 所以实验三既要用到实验一的语法树又要用到实验二的生成符号表的相关操作，而为了最大程度地减少代码的更改量，我们最终的解决方案是，将两个实验的数据结构结合起来，封装成一个更高层次的结构体作为所有Variable的type。具体如下：

```c
struct Node_ { //存放语法树的节点
	char *name;
	enum NodeType nodeType;
	char stringValue[300];
	int numValue;
	float numValueF;
	int childNum;
	Node child[8];
};
```

```c
struct FieldList_ //存放实验二中变量的节点
{
	char name[maxIdLength];
	Type type;
	FieldList next;
	char isLeftValue[10];
	char isFollowEqual[10];
	int line;
	enum { DEFVAR, DECVAR } status;
};

struct FuncList_ //存放实验二中函数的节点
{
	enum { DEF, DEC } status;
	Type return_type;
	char name[maxIdLength];
	FieldList parameters;
	FuncList next;
	int line;
};
```

```c
struct SyntaxFieldNode_ //结合实验一实验二的结构体
{
	FieldList field;
	Node node;
};

struct SyntaxFuncNode_
{
	FuncList func;
	Node node;
};
```

```c
typedef struct YYSTYPE //更新后的YYSTYPE
{
	int type_int;
	float type_float;
	SyntaxCharNode type_char;
	SyntaxTypeNode type_type;
	SyntaxFieldNode type_field;
	SyntaxFuncNode type_func;
} YYSTYPE;
```

这样就可以完美地将实验一实验二的结构体融合到一起，生成语法树的同时建立符号表等。

因为一开始也思考过边建立语法树边生成中间代码，但是后来发现有一些综合属性的参数值并不能传递下去，而且规约方向往往与生成中间代码方向相反，遂放弃， 改为建好语法树后对于语法树进行递归翻译。