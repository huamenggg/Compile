#include "generateSyntaxTree.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct InterCode_* InterCode;
int labelNum;
int tempNum;

enum OperandKind { VARIABLE, CONSTANT, FUNCTION, ADDRESS, TEMPORLABEL, BADD, BMINUS, BSTAR, BDIV, RE, ARGUMENT, WRITE };
enum InterCodeKind { ADDI, SUBI, MULI, DIVI, ASSIGNI, RETURNI, LABEL, GOTO, COND1, READI, CALLI, WRITEI, ARG };

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
		struct { Operand t1, op, t2, label;} cond1;
		struct { Operand func, place;} call;
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
Operand GenerateOperandFunc(FuncList func);
Operand GenerateOperandArg(FieldList a);
Operand GenerateOperandWrite(FieldList a);

/* Operation of InterCode */
InterCode GenerateInterCodeAssign(Operand right, Operand left);
InterCode GenerateInterCodeOp(enum InterCodeKind kind, Operand result, Operand op1, Operand op2);
InterCode GenerateInterCodeReturnOrLabel(enum InterCodeKind kind, Operand op);
InterCode GenerateInterCodeCond1(enum InterCodeKind kind, Operand t1, Operand op, Operand t2, Operand label);
InterCode GenerateInterCodeReadOrWrite(enum InterCodeKind kind, Operand op);
InterCode GenerateInterCodeCall(Operand func, Operand place);
InterCode GenerateInterCodeArg(Operand op);

/* Operation of InterCodes */
void InitialInterCodes();
int DeleteInterCodes(InterCodes del);

void writeToFile(FILE *f);
void writeOperand(Operand op, FILE *f);

void generateInterCode();

/* generate new temp variable */
void new_temp(char* tempName);

void new_label(char* label);

/* translate intermedia code */
InterCodes translate_CompSt(Node node);
InterCodes translate_Exp(Node node, char* place);
InterCodes translate_Cond(Node node, char* label1, char* label2);
InterCodes translate_Program(Node node);
InterCodes translate_Stmt(Node node);
InterCodes translate_Args(Node node);
InterCodes singleCode(InterCode ic);


InterCodes codeAdd(InterCodes a, InterCodes b);

