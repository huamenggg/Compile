#include "semantic.h"
#include "intermediate.h"

Operand GenerateOperand(enum OperandKind kind, int value) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = kind;
	op->u.value = value;
	return op;
}

Operand GenerateOperandVariable(FieldList f) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = VARIABLE;
	op->u.symbol = f;
	return op;
}

Operand GenerateOperandFunction(FuncList f) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = FUNCTION;
	op->u.func = f;
	return op;
}

Operand GenerateOperandTemp(char* c) {
	if(c == NULL) return NULL;
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = TEMPORLABEL;
	strcpy(op->u.name, c);
	return op;
}

InterCode GenerateInterCodeAssign(Operand right, Operand left) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = ASSIGNI;
	ic->u.assign.right = right;
	ic->u.assign.left = left;
	return ic;
}

InterCode GenerateInterCodeOp(enum InterCodeKind kind, Operand result, Operand op1, Operand op2) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = kind;
	ic->u.binop.result = result;
	ic->u.binop.op1 = op1;
	ic->u.binop.op2 = op2;
	return ic;
}

InterCode GenerateInterCodeReturnOrLabel(enum InterCodeKind kind, Operand op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = kind;
	ic->u.value = op;
	return ic;
}

InterCodes singleCode(InterCode ic) {
	InterCodes ics = (InterCodes)malloc(sizeof(struct InterCodes_));
	ics->code = ic;
	ics->pre = ics->next = NULL;
	ics->tail = ics;
	return ics;
}

void InitialInterCodes() {
	labelNum = 1;
	tempNum = 1;
}

int DeleteInterCodes(InterCodes del) {
	del->pre->next = del->next;
	del->next->pre = del->pre;
	free(del);
	return 1;
}

void writeToFile(FILE *f) {
	//TODO need to finish
	InterCodes out = head;
	while(out != NULL) {
		if(out->code->kind == ASSIGNI) {
			//TODO print need to be packaged
			if(out->code->u.assign.left->kind == VARIABLE)
				printf("%s", out->code->u.assign.left->u.symbol->name);
			else if(out->code->u.assign.left->kind == TEMPORLABEL)
				printf("%s", out->code->u.assign.left->u.name);
			else
				printf("%d", out->code->u.assign.left->u.value);
			printf(" := ");
			if(out->code->u.assign.right->kind == VARIABLE)
				printf("%s\n", out->code->u.assign.right->u.symbol->name);
			else if(out->code->u.assign.right->kind == TEMPORLABEL)
				printf("%s\n", out->code->u.assign.right->u.name);
			else
				printf("%d\n", out->code->u.assign.right->u.value);
		}
		else if(out->code->kind == RETURNI) {
			printf("RETURN %s\n", out->code->u.value->u.name);
		}
		out = out->next;
	}
	return;
}

InterCodes translate_Stmt(Node node) {
	switch(node->childNum) {
		case 1: { /* Stmt -> CompSt */
				return translate_CompSt(node->child[0]);
			}
		case 2: { /* Stmt -> Exp SEMI */
				return translate_Exp(node->child[0], NULL);
			}
		case 3: { /* Stmt -> RETURN Exp SEMI */
				char t1[20];
				new_temp(t1);
				InterCodes code1 = translate_Exp(node->child[1], t1);
				Operand op = GenerateOperandTemp(t1);
				InterCode ic = GenerateInterCodeReturnOrLabel(RETURNI, op);	
				InterCodes code2 = singleCode(ic);
				return codeAdd(code1, code2);
			}
		case 5: {
				//TODO: distinct if and while
			}
		case 7: {
				char label1[20];
				char label2[20];
				char label3[20];
				new_label(label1);
				new_label(label2);
				new_label(label3);
				InterCodes code1 = translate_Cond(node->child[2], label1, label2);
				InterCodes code2 = translate_Stmt(node->child[4]);
				InterCodes code3 = translate_Stmt(node->child[6]);
				Operand op1 = GenerateOperandTemp(label1);
				Operand op2 = GenerateOperandTemp(label2);
				Operand op3 = GenerateOperandTemp(label3);
				InterCode ic1 = GenerateInterCodeReturnOrLabel(LABEL, op1);
				InterCode ic2 = GenerateInterCodeReturnOrLabel(LABEL, op2);
				InterCode ic3 = GenerateInterCodeReturnOrLabel(LABEL, op3);
				InterCode ic4 = GenerateInterCodeReturnOrLabel(GOTO, op3);
				InterCodes code4 = singleCode(ic1);
				InterCodes code5 = singleCode(ic2);
				InterCodes code6 = singleCode(ic3);
				InterCodes code7 = singleCode(ic4);
				codeAdd(code1, code4);
				codeAdd(code1, code2);
				codeAdd(code1, code7);
				codeAdd(code1, code5);
				codeAdd(code1, code3);
				codeAdd(code1, code6);
				return code1;
			}
		default: { fprintf(stderr, "Error in translate_Stmt\n"); exit(0); }
	}
}

InterCodes translate_Cond(Node node, char* label1, char* labe2) {
	return NULL;
}

InterCodes translate_CompSt(Node node) {
	return NULL;
}

InterCodes translate_Exp(Node node, char* place) {
	Operand op = GenerateOperandTemp(place);
	switch(node->childNum) {
		case 1: {
				if(strcmp(node->child[0]->name, "ID") == 0) {
					if(op) {
						//Different from case3
						//this rule is Exp->ID
						//so node->child[0] is ID
						FieldList f = getSymbol(node->child[0]->stringValue);
						Operand op1 = GenerateOperandVariable(f);
						InterCode ic = GenerateInterCodeAssign(op1, op);
						return singleCode(ic);
					}
					return NULL;
				}
				else if(strcmp(node->child[0]->name, "INT") == 0) {
					if(op) {
						Operand op1 = GenerateOperand(CONSTANT, node->child[0]->numValue);
						InterCode ic = GenerateInterCodeAssign(op1, op);
						return singleCode(ic);
					}
					return NULL;
				}
			}
		case 2: {
				break;
			}
		case 3: {
				if(strcmp(node->child[1]->name, "ASSIGNOP") == 0) {
					// this rule is Exp->Exp = Exp
					// so node is Exp(left), Exp->child[0] is Exp(right)
					// but what we need is Exp->ID
					// we need ID->stringValue
					// so need node->child[0]->child[0]
					FieldList f = getSymbol(node->child[0]->child[0]->stringValue);
					char t1[20];
					new_temp(t1);
					InterCodes code1 = translate_Exp(node->child[2], t1);
					Operand op1 = GenerateOperandVariable(f);
					Operand op2 = GenerateOperandTemp(t1);
					InterCode ic = GenerateInterCodeAssign(op2, op1);
					InterCodes code2 = singleCode(ic);
					if(op) {
						InterCode ic1 = GenerateInterCodeAssign(op1, op);
						InterCodes code3 = singleCode(ic1);
						code2 =  codeAdd(code2, code3);
					}
					return codeAdd(code1, code2);
				}
				//TODO need to finish ADD etc.
			}
		default: { printf("Error in translate_Exp\n"); exit(0); }
	}
}

InterCodes translate_Program(Node node) {
	//TODO need to finish
	InterCodes c1 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[0]);	
	InterCodes c2 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[0]);	
	InterCodes c3 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[1]->child[0]);	
	InterCodes c4 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[1]->child[1]->child[0]);	
	return codeAdd(c1, codeAdd(c2, codeAdd(c3, c4)));
}

void generateInterCode() {
	if(root == NULL) {
		printf("This is an empty syntax tree\n");
		return;
	}
	head = translate_Program(root);
	tail = head->tail;
}

void new_temp(char* tempName) {
	sprintf(tempName, "t%d", tempNum);
	tempNum++;
}

void new_label(char* label) {
	sprintf(label, "label%d", labelNum);
	labelNum++;
}

InterCodes codeAdd(InterCodes a, InterCodes b) {
	if(!a) return b;
	if(!b) return a;
	a->tail->next = b;
	b->pre = a->tail;
	a->tail = b->tail;
	return a;
}
