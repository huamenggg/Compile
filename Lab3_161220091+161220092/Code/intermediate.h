#include "generateSyntaxTree.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct InterCode_* InterCode;
int labelNum;
int tempNum;
int paramNum;

enum OperandKind { VARIABLE, CONSTANT, FUNCTION, TEMPORLABEL, BADD, BMINUS, BSTAR, BDIV, RE, ARGUMENT, WRITE, PARAM, CALL, GETADDRESS, GETVALUE};
enum InterCodeKind { ADDI, SUBI, MULI, DIVI, ASSIGNI, RETURNI, LABEL, GOTO, COND1, READI, CALLI, WRITEI, ARG, DECI, PARAMI, FUNCTIONI };

struct Operand_ {
	enum OperandKind kind;
	union {
		FieldList symbol;
		int value;
		FuncList func;
		char name[50];
		struct { Operand a, b; } bi;
	} u;
};

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
};

struct InterCodes_ {
	InterCode code;
	InterCodes pre, next;
	InterCodes tail;
};

InterCodes head;
InterCodes tail;

/* Operation of Operand */
Operand GenerateOperand(enum OperandKind kind, int value);
Operand GenerateOperandVariable(FieldList f);
Operand GenerateOperandFunction(FuncList f);
Operand GenerateOperandTemp(char* c);
Operand GenerateOperandBi(enum OperandKind kind, Operand a, Operand b);
Operand GenerateOperandRELOP(char *c);
Operand GenerateOperandCall(FuncList func);
Operand GenerateOperandArg(FieldList a);
Operand GenerateOperandWrite(FieldList a);
Operand GenerateOperandParam(char* c);
Operand GenerateOperandFunc(char* c);
Operand GenerateOperandGetAddress(FieldList f); //t1 = &t2; this store t2;
Operand GenerateOperandGetValue(FieldList f); //t1 = *t2; this store t2;

/* Operation of InterCode */
InterCode GenerateInterCodeAssign(Operand right, Operand left);
InterCode GenerateInterCodeOp(enum InterCodeKind kind, Operand result, Operand op1, Operand op2);
InterCode GenerateInterCodeReturnOrLabel(enum InterCodeKind kind, Operand op);
InterCode GenerateInterCodeCond1(enum InterCodeKind kind, Operand t1, Operand op, Operand t2, Operand label);
InterCode GenerateInterCodeReadOrWrite(enum InterCodeKind kind, Operand op);
InterCode GenerateInterCodeCall(Operand func, Operand place);
InterCode GenerateInterCodeArg(Operand op);
InterCode GenerateInterCodeDec(Operand op, int size);
InterCode GenerateInterCodeParam(Operand op);
InterCode GenerateInterCodeFunc(Operand op);


/* Operation of InterCodes */
void InitialInterCodes();
int DeleteInterCodes(InterCodes del);

void writeToFile(FILE *f);
void writeOperand(Operand op, FILE* f);

void generateInterCode();

/* generate new temp variable */
void new_temp(char* tempName);

void new_label(char* label);

void new_param(char* param);
/* translate intermedia code */
InterCodes translate_CompSt(Node node);
InterCodes translate_Exp(Node node, char* place);
InterCodes translate_Cond(Node node, char* label1, char* label2);
InterCodes translate_Program(Node node);
InterCodes translate_Stmt(Node node);
InterCodes translate_Args(Node node, char* place);
InterCodes translate_ExtDefList(Node node);
InterCodes translate_ExtDef(Node node);
InterCodes translate_ExtDecList(Node node);
InterCodes translate_VarDec(Node node);
InterCodes translate_FunDec(Node node);
InterCodes translate_VarList(Node node);
InterCodes translate_StmtList(Node node);
InterCodes translate_DefList(Node node);
InterCodes translate_Def(Node node);
InterCodes translate_DecList(Node node);
InterCodes translate_Dec(Node node);
InterCodes singleCode(InterCode ic);

InterCodes codeAdd(InterCodes a, InterCodes b);

int CalcStructSize(FieldList f);
