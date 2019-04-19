#include "stdio.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymbolList_* SymbolList;

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE } kind;
	union
	{
		//basic type
		int basic;
		//array type and size
		struct {Type elem; int size; } array;
		//struct
		FieldList structure;
	} u;
};

struct FieldList_
{
	char* name;
	Type type;
	FieldList tail;
};

struct SymbolList_
{
	char* name;
	Type type;
	SymbolList next;
};

Type typeList[100];
SymbolList symbolList[300];

int IsId(char ch);
char* Filter(char* string);
