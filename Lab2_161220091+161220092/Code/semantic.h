#include <stdio.h>
#include "string.h"
#include <stdlib.h>

#ifndef YST_H
#define YST_H
#define hashLength 300
#define maxIdLength 100
#define RED "\033[0;32;31m"
#define NONE "\033[m"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

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
	} u;
};

struct FieldList_
{
	char name[maxIdLength];
	Type type;
	FieldList next;
};

typedef struct YYSTYPE
{
	int type_int;
	float type_float;
	char* type_char;
	Type type_type;
	FieldList type_field;
} YYSTYPE;

Type typeList[100];
FieldList symbolList[hashLength];
int typeLength;

void initSemantic();
int IsId(char ch);
char* Filter(char* string);
int getHashIndex(char* string);
FieldList insertSymbol(FieldList field);
FieldList getSymbol(char* name);

Type getTypeAddress(char* typeName);
Type insertType(Type type);
Type generateType(char* name, FieldList head);

FieldList generateField(char* name, Type type);
#endif
