#include "stdio.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

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

int IsId(char ch);
char* Filter(char* string);
