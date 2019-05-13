#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct InterCode_* InterCode;

struct Operand_ {
	enum { VARIABLE, CONSTANT, ADDRESS } kind;
	union {
		int var_no;
		int value;
	} u;
};

struct InterCode_ {
	enum { ADD, SUB, MUL, DIV, ASSIGN} kind;
	union {
		struct { Operand result, op1, op2; } binop;
		struct { Operand right, left; } assign;
	} u;
};

struct InterCodes_ {
	InterCode code;
	InterCodes pre, next;
};

InterCodes head;
InterCodes tail;

/* Operation of Operand */
// kind { variable:0 constant:1 address:2 } */
Operand GenerateOperand(int kind, int value);

/* Operation of InterCode */
InterCode GenerateInterCodeAssign(Operand right, Operand left);
// op { + is 0, - is 1, * is 2, / is 3 }
InterCode GenerateInterCodeOp(Operand result, Operand op1, Operand op2, int op);

/* Operation of InterCodes */
void InitialInterCodes();
int InsertInterCodes(InterCode insert);
int DeleteInterCodes(InterCodes del);

void writeToFile(FILE *f);
