#include "intermediate.h"

Operand GenerateOperand(int kind, int value) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	switch(kind) {
		case 0: op->kind = VARIABLE; op->u.var_no = value; break;
		case 1: op->kind = CONSTANT; op->u.value = value; break;
 		case 2: op->kind = ADDRESS; op->u.value = value; break;
		default: printf("Error kind in GenerateOperand\n");
	}
	return op;
}

InterCode GenerateInterCodeAssign(Operand right, Operand left) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = ASSIGNI;
	ic->u.assign.right = right;
	ic->u.assign.left = left;
	return ic;
}

InterCode GenerateInterCodeOp(Operand result, Operand op1, Operand op2, int op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = op;
	ic->u.binop.result = result;
	ic->u.binop.op1 = op1;
	ic->u.binop.op2 = op2;
	return ic;
}

void InitialInterCodes() {
	head = (InterCodes)malloc(sizeof(struct InterCodes_));
	tail = (InterCodes)malloc(sizeof(struct InterCodes_));
	head->pre = tail->next = NULL;
	head->next = tail;
	tail->pre = head;
}

int InsertInterCodes(InterCode insert) {
	InterCodes ics = (InterCodes)malloc(sizeof(struct InterCodes_));
	ics->code = insert;
	ics->pre = tail->pre;
	ics->next = tail;
	tail->pre->next = ics;
	tail->pre = ics;
	return 1;
}

int DeleteInterCodes(InterCodes del) {
	del->pre->next = del->next;
	del->next->pre = del->pre;
	free(del);
	return 1;
}

void writeToFile(FILE *f) {
	InterCodes out = head->next;
	while(out != tail) {
		if(out->code->kind == ASSIGNI) {
			printf("%d = %d\n", out->code->u.assign.right->u.var_no, out->code->u.assign.left->u.var_no);
		}
		else
			printf("%d = %d %d %d\n", out->code->u.binop.result->u.var_no, out->code->u.binop.op1->u.var_no, out->code->kind, out->code->u.binop.op2->u.var_no);
		out = out->next;
	}
	return;
}
