#include "generateSyntaxTree.h"

#ifndef YST_H
#define YST_H
#define hashLength 300
#define maxIdLength 100
#define RED "\033[0;32;31m"
#define NONE "\033[m"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncList_* FuncList;
typedef struct SyntaxFieldNode_* SyntaxFieldNode;
typedef struct SyntaxFuncNode_* SyntaxFuncNode;
typedef struct SyntaxTypeNode_* SyntaxTypeNode;
typedef struct SyntaxCharNode_* SyntaxCharNode;

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE } kind;
	char name[maxIdLength];
	union
	{
		//basic type
		int basic;
		//array type and size
		struct {Type elem; int size; } array;
		//struct
		FieldList structure;
		//function

	} u;
};

struct FieldList_
{
	char name[maxIdLength];
	Type type;
	FieldList next;
	char isLeftValue[10];
	char isFollowEqual[10];
	int line;
	enum { DEFVAR, DECVAR } status;
};

struct FuncList_
{
	enum { DEF, DEC } status;
	Type return_type;
	char name[maxIdLength];
	FieldList parameters;
	FuncList next;
	int line;
};

struct SyntaxTypeNode_
{
	Type type;
	Node node;
};

struct SyntaxFieldNode_
{
	FieldList field;
	Node node;
};

struct SyntaxFuncNode_
{
	FuncList func;
	Node node;
};

struct SyntaxCharNode_
{
	char* c;
	Node node;
};

typedef struct YYSTYPE
{
	int type_int;
	float type_float;
	SyntaxCharNode type_char;
	SyntaxTypeNode type_type;
	SyntaxFieldNode type_field;
	SyntaxFuncNode type_func;
} YYSTYPE;

Type typeList[hashLength];
FieldList symbolList[hashLength];
FuncList funcList[hashLength];
FieldList errorSymbol[hashLength];
FuncList errorFunc[hashLength];
FieldList argList[hashLength];
int typeLength;
int errorLength;
int errorFuncLength;
int argLength;

void initSemantic();
int IsId(char ch);
char* Filter(char* string);

//hash_pjw
int getHashIndex(char* string);

//symbol table
FieldList insertSymbol(FieldList field, int line, int ifPrint);
FieldList getSymbol(char* name);

//type table
Type getTypeAddress(char* typeName, int line, int ifPrint);
Type insertType(Type type, int line);
Type generateTypeArray(int size);
Type generateType(char* name, FieldList head);
FieldList generateField(char* name, Type type);

int addArg(char *name);
void clearArgList();
//ArgList generateArg(char* name);
//ArgList insertArg(ArgList arg);

//function table
FuncList insertFunc(FuncList Func, int line);
FuncList generateFunc(char* name, FieldList parameter, Type return_type);
FuncList getFuncAddress(char* funcName);

//Equal
int checkFuncEqual(FuncList f1, FuncList f2, int line, int ifPrint);
int checkParameter(char* funcName, FieldList f1, FieldList f2, int line, int ifPrint);
int typeEqual(Type type1, Type type2);

//error check
FieldList checkExp(FieldList exp1, FieldList exp2, char* error, int line);
FieldList checkExpID(char* exp, int line);
FieldList checkExpArray(FieldList array, FieldList index, int line);
FieldList checkExpStruct(FieldList exp1, char* exp2, int line);
FieldList checkExpFunc(char* funcName, FieldList parameter, int line);
void checkIfType(FieldList f, int line);
void checkReturnType(Type request, FieldList f);

//others
void Split0(char* string);
void printStructError();
void printVarError();
void printFuncError();
void printStructFollowEqualError(FieldList f);
void changeFieldToDec(FieldList f);
FieldList copyField(FieldList exp);
#endif
