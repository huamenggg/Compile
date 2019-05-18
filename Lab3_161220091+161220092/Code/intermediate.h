#include "generateSyntaxTree.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct InterCode_* InterCode;
int labelNum;
int tempNum;

enum OperandKind { VARIABLE, CONSTANT, FUNCTION, ADDRESS, TEMPORLABEL };
enum InterCodeKind { ADDI, SUBI, MULI, DIVI, ASSIGNI, RETURNI, LABEL, GOTO };

struct Operand_ {
	enum OperandKind kind;
	union {
		FieldList symbol;
		int value;
		FuncList func;
		char name[50];
	} u;
};

struct InterCode_ {
	enum InterCodeKind kind;
	union {
		struct { Operand result, op1, op2; } binop;
		struct { Operand right, left; } assign;
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

/* Operation of InterCode */
InterCode GenerateInterCodeAssign(Operand right, Operand left);
InterCode GenerateInterCodeOp(enum InterCodeKind kind, Operand result, Operand op1, Operand op2);
InterCode GenerateInterCodeReturnOrLabel(enum InterCodeKind kind, Operand op);

/* Operation of InterCodes */
void InitialInterCodes();
int DeleteInterCodes(InterCodes del);

void writeToFile(FILE *f);

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
InterCodes singleCode(InterCode ic);

InterCodes codeAdd(InterCodes a, InterCodes b);

