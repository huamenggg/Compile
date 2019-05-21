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

Operand GenerateOperandBi(enum OperandKind kind, Operand a, Operand b) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = kind;
	op->u.bi.a = a;
	op->u.bi.b = b;
	return op;
}

Operand GenerateOperandRELOP(char *c){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = RE;
	strcpy(op->u.name, c);
	return op;
}

Operand GenerateOperandFunc(FuncList func) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = FUNCTION;
	op->u.func = func;
	return op;
}

Operand GenerateOperandArg(FieldList a) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = ARGUMENT;
	op->u.symbol = a;
	return op;
}

Operand GenerateOperandWrite(FieldList a) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = WRITE;
	op->u.symbol = a;
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

InterCode GenerateInterCodeCond1(enum InterCodeKind kind, Operand t1, Operand op, Operand t2, Operand label) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = kind;
	ic->u.cond1.t1 = t1;
	ic->u.cond1.op = op;
	ic->u.cond1.t2 = t2;
	ic->u.cond1.label = label;
	return ic;
}

InterCode GenerateInterCodeReadOrWrite(enum InterCodeKind kind, Operand op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = kind;
	ic->u.value = op;
	return ic;
}

InterCode GenerateInterCodeCall(Operand func, Operand place) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = CALLI;
	ic->u.call.func = func;
	ic->u.call.place = place;
	return ic;
}

InterCode GenerateInterCodeArg(Operand op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = ARG;
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

void writeOperand(Operand op) {
	if(op->kind == VARIABLE)
		printf("%s", op->u.symbol->name);
	else if(op->kind == TEMPORLABEL)
		printf("%s", op->u.name);
	else if(op->kind == BADD || op->kind == BMINUS || op->kind == BSTAR || op->kind == BDIV) {
		Operand op1 = op->u.bi.a;
		Operand op2 = op->u.bi.b;
		printf("%s", op1->u.name);
		if(op->kind == BADD)
			printf(" + ");
		if(op->kind == BMINUS)
			printf(" - ");
		if(op->kind == BSTAR)
			printf(" * ");
		if(op->kind == BDIV)
			printf(" / ");
		printf("%s", op2->u.name);
	}
	else if(op->kind == CONSTANT)
		printf("#%d", op->u.value);
	else if(op->kind == RE)
		printf("%s", op->u.name);
	else if(op->kind == FUNCTION)
		printf("CALL %s", op->u.func->name);
	else if(op->kind == ARGUMENT || op->kind == WRITE)
		printf("%s", op->u.symbol->name);
	else 
		printf("%d", op->u.value);
}

void writeToFile(FILE *f) {
	//TODO need to finish
	InterCodes out = head;
	while(out != NULL) {
		InterCode ic = out->code;
		if(ic->kind == ASSIGNI) {
			//TODO print need to be packaged
			Operand op1 = ic->u.assign.left;
			Operand op2 = ic->u.assign.right;
			writeOperand(op1);
			printf(" := ");
			writeOperand(op2);	
			printf("\n");
		}
		else if(ic->kind == RETURNI) {
			printf("RETURN ");
			writeOperand(ic->u.value);
			printf("\n");
		}
		else if(ic->kind == COND1) {
			Operand t1 = ic->u.cond1.t1;
			Operand op = ic->u.cond1.op;
			Operand t2 = ic->u.cond1.t2;
			Operand label = ic->u.cond1.label;
			printf("IF ");
			writeOperand(t1);
			printf(" ");
			writeOperand(op);
			printf(" ");
			writeOperand(t2);
			printf(" GOTO ");
			writeOperand(label);
			printf("\n");

		}
		else if(ic->kind == GOTO) {
			printf("GOTO ");
			writeOperand(ic->u.value);
			printf("\n");
		}
		else if(ic->kind == LABEL) {
			printf("LABEL ");
			writeOperand(ic->u.value);
			printf(" :\n");
		}
		else if(ic->kind == READI) {
			printf("READ ");
			writeOperand(ic->u.value);
			printf("\n");
		}
		else if(ic->kind == CALLI) {
			Operand func = ic->u.call.func;
			Operand place = ic->u.call.place;
			writeOperand(place);
			printf(" := CALL %s\n", func->u.func->name);
		}
		else if(ic->kind == WRITEI) {
			printf("WRITE ");
			writeOperand(ic->u.value);
			printf("\n");
		}
		else if(ic->kind == ARG) {
			printf("ARG ");
			writeOperand(ic->u.value);
			printf("\n");
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
				if(strcmp(node->child[0]->name, "IF") == 0) {
					char label1[20];
					char label2[20];
					new_label(label1);
					new_label(label2);
					InterCodes code1 = translate_Cond(node->child[2], label1, label2);
					InterCodes code2 = translate_Stmt(node->child[4]);	
					Operand op1 = GenerateOperandTemp(label1);
					Operand op2 = GenerateOperandTemp(label2);
					InterCode ic1 = GenerateInterCodeReturnOrLabel(LABEL, op1);
					InterCode ic2 = GenerateInterCodeReturnOrLabel(LABEL, op2);
					InterCodes code11 = singleCode(ic1);
					InterCodes code22 = singleCode(ic2);
					codeAdd(code1, code11);
					codeAdd(code1, code2);
					codeAdd(code1, code22);
					return code1;
				}
				else if(strcmp(node->child[0]->name, "WHILE") == 0) {
					char label1[20];
					char label2[20];
					char label3[20];
					new_label(label1);
					new_label(label2);
					new_label(label3);
					InterCodes code1 = translate_Cond(node->child[2], label2, label3);
					InterCodes code2 = translate_Stmt(node->child[4]);
					Operand op1 = GenerateOperandTemp(label1);
					Operand op2 = GenerateOperandTemp(label2);
					Operand op3 = GenerateOperandTemp(label3);
					InterCode ic1 = GenerateInterCodeReturnOrLabel(LABEL, op1);
					InterCode ic2 = GenerateInterCodeReturnOrLabel(LABEL, op2);
					InterCode ic3 = GenerateInterCodeReturnOrLabel(LABEL, op3);
					InterCode ic4 = GenerateInterCodeReturnOrLabel(GOTO, op1);
					InterCodes code11 = singleCode(ic1);
					InterCodes code12 = singleCode(ic4);
					InterCodes code22 = singleCode(ic2);
					InterCodes code33 = singleCode(ic3);
					codeAdd(code11, code1);
					codeAdd(code11, code22);
					codeAdd(code11, code2);
					codeAdd(code11, code12);
					codeAdd(code11, code33);
					return code11;
				}	
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

InterCodes translate_Cond(Node node, char* label1, char* label2) {
	new_label(label1);
	new_label(label2);	
	Operand opt = GenerateOperandTemp(label1);
	Operand opf = GenerateOperandTemp(label2);
	switch(node->childNum) {
		case 2: {
				return translate_Cond(node->child[1], label2, label1);
			}	
		case 3: {
				if(strcmp(node->child[1]->name, "RELOP") == 0) {
					char t1[20];
					char t2[20];
					new_temp(t1);
					new_temp(t2);
					Operand op1 = GenerateOperandTemp(t1);
					Operand op2 = GenerateOperandTemp(t2);
					InterCodes code1 = translate_Exp(node->child[0], t1);
					InterCodes code2 = translate_Exp(node->child[2], t2);
					Operand op = GenerateOperandRELOP(node->child[1]->stringValue);
					InterCode ic1 = GenerateInterCodeCond1(COND1, op1, op, op2, opt);
					InterCode ic2 = GenerateInterCodeReturnOrLabel(GOTO, opf);	
					InterCodes code3 = singleCode(ic1);
					InterCodes code4 = singleCode(ic2);
					codeAdd(code1, code2);
					codeAdd(code1, code3);
					codeAdd(code1, code4);
					return code1;
				}
				else if(strcmp(node->child[1]->name, "AND") == 0) {
					//label:new_label
					//label1:label_true
					//label2:label_false
					char label[20];
					new_label(label);
					Operand op = GenerateOperandTemp(label);
					InterCodes code1 = translate_Cond(node->child[0], label, label2);
					InterCodes code2 = translate_Cond(node->child[2], label1, label2);
					InterCode ic = GenerateInterCodeReturnOrLabel(LABEL, op);
					InterCodes code3 = singleCode(ic);
					codeAdd(code1, code3);
					codeAdd(code1, code2);
					return code1;
				}
				else if(strcmp(node->child[1]->name, "OR") == 0) {
					char label[20];
					new_label(label);
					Operand op = GenerateOperandTemp(label);
					InterCodes code1 = translate_Cond(node->child[0], label1, label);
					InterCodes code2 = translate_Cond(node->child[2], label1, label2);
					InterCode ic = GenerateInterCodeReturnOrLabel(LABEL, op);
					InterCodes code3 = singleCode(ic);
					codeAdd(code1, code3);
					codeAdd(code1, code2);
					return code1;
				}
			}
		default: {
				char t1[20];
				new_temp(t1);
				InterCodes code1 = translate_Exp(node, t1);
				Operand op1 = GenerateOperandTemp(t1);
				Operand op2 = GenerateOperandRELOP("!=");
				Operand op3 = GenerateOperand(CONSTANT, 0);
				InterCode ic1 = GenerateInterCodeCond1(COND1, op1, op3, op2, opt);
				InterCode ic2 = GenerateInterCodeReturnOrLabel(GOTO, opf);
			 	InterCodes code2 = singleCode(ic1);
				InterCodes code3 = singleCode(ic2);
				codeAdd(code1, code2);
				codeAdd(code1, code3);
				return code1;
			 }

	}
	//return NULL;
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
				char label1[20];
				char label2[20];
				new_label(label1);
				new_label(label2);
				Operand op1 = GenerateOperand(CONSTANT, 0);
				Operand op2 = GenerateOperandTemp(label1);
				Operand op3 = GenerateOperandTemp(label2);
				Operand op4 = GenerateOperand(CONSTANT, 1);
				InterCode ic1 = GenerateInterCodeAssign(op1, op);
				InterCode ic2 = GenerateInterCodeReturnOrLabel(LABEL, op2);
				InterCode ic3 = GenerateInterCodeReturnOrLabel(LABEL, op3);
				InterCode ic4 = GenerateInterCodeAssign(op4, op);
				InterCodes code0 = singleCode(ic1);//[place:=0]
				InterCodes code1 = translate_Cond(node, label1, label2);
				InterCodes code2 = singleCode(ic2);//label1
				InterCodes code3 = singleCode(ic3);//label2
				InterCodes code4 = singleCode(ic4);//[place:=1]
				codeAdd(code2, code4);
				codeAdd(code0, code1);
				codeAdd(code0, code2);
				codeAdd(code0, code3);
				return 0;
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
				else if(strcmp(node->child[1]->name, "PLUS") == 0 || 
						strcmp(node->child[1]->name, "MINUS") == 0 ||
						strcmp(node->child[1]->name, "STAR") == 0 ||
						strcmp(node->child[1]->name, "DIV") == 0
						) {
					char t1[20];
					char t2[20];
					new_temp(t1);
					new_temp(t2);
					InterCodes code1 = translate_Exp(node->child[0], t1);
					InterCodes code2 = translate_Exp(node->child[2], t2);
					Operand op1 = GenerateOperandTemp(t1);
					Operand op2 = GenerateOperandTemp(t2);
					Operand op3 = GenerateOperandBi(BADD, op1, op2);
					if(strcmp(node->child[1]->name, "PLUS") == 0)
						op3 = GenerateOperandBi(BADD, op1, op2);
					if(strcmp(node->child[1]->name, "MINUS") == 0)
						op3 = GenerateOperandBi(BMINUS, op1, op2);
					if(strcmp(node->child[1]->name, "STAR") == 0)
						op3 = GenerateOperandBi(BSTAR, op1, op2);
					if(strcmp(node->child[1]->name, "DIV") == 0)
						op3 = GenerateOperandBi(BDIV, op1, op2);
					InterCode ic1 = GenerateInterCodeAssign(op3, op);
					InterCodes code3 = singleCode(ic1);
					codeAdd(code1, code2);
					codeAdd(code1, code3);
					return code1;			
				}
				else if(strcmp(node->child[1]->name, "RELOP") == 0 
					||strcmp(node->child[1]->name, "AND") == 0 
					||strcmp(node->child[1]->name, "OR") == 0) {
					char label1[20];
					char label2[20];
					new_label(label1);
					new_label(label2);
					Operand op1 = GenerateOperand(CONSTANT, 0);
					Operand op2 = GenerateOperandTemp(label1);
					Operand op3 = GenerateOperandTemp(label2);
					Operand op4 = GenerateOperand(CONSTANT, 1);
					InterCode ic1 = GenerateInterCodeAssign(op1, op);
					InterCode ic2 = GenerateInterCodeReturnOrLabel(LABEL, op2);
					InterCode ic3 = GenerateInterCodeReturnOrLabel(LABEL, op3);
					InterCode ic4 = GenerateInterCodeAssign(op4, op);
					InterCodes code0 = singleCode(ic1);//[place:=0]
					InterCodes code1 = translate_Cond(node, label1, label2);
					InterCodes code2 = singleCode(ic2);//label1
					InterCodes code3 = singleCode(ic3);//label2
					InterCodes code4 = singleCode(ic4);//[place:=1]
					codeAdd(code2, code4);
					codeAdd(code0, code1);
					codeAdd(code0, code2);
					codeAdd(code0, code3);
					return code0;
				}
				else if(strcmp(node->child[0]->name, "ID") == 0) {
					FuncList func1 = getFuncAddress(node->child[0]->stringValue);
					if(strcmp(func1->name, "read") == 0) {
						InterCode ic1 = GenerateInterCodeReadOrWrite(READI, op);
						InterCodes code1 = singleCode(ic1);
						return code1;
					}
					Operand func2 = GenerateOperandFunc(func1);
					InterCode ic2 = GenerateInterCodeCall(func2, op);
					InterCodes code2 = singleCode(ic2);
				       	return code2;
				}
			}
		case 4: {
				FuncList func1 = getFuncAddress(node->child[0]->stringValue);
				clearArgList();
				InterCodes code1 = translate_Args(node->child[2]);
				if(strcmp(func1->name, "write") == 0) {
					Operand opa1 = GenerateOperandWrite(argList[0]);
					InterCode ic1 = GenerateInterCodeReadOrWrite(WRITEI, opa1);
					InterCodes code2 = singleCode(ic1);
					codeAdd(code1, code2);
					return code1;
				}
				if(argLength == 0) {
					printf("Error in translate_Exp\n");
					exit(0);
				}
				
				Operand opa2 = GenerateOperandArg(argList[0]);
				InterCode ic2 = GenerateInterCodeArg(opa2);
				InterCodes code2 = singleCode(ic2);
				
				if(argLength > 1) {
					for(int i = 1; i < argLength; i++) {
						Operand opai = GenerateOperandArg(argList[i]);
						InterCode ici = GenerateInterCodeArg(opai);
						InterCodes codei = singleCode(ici);
						codeAdd(code2, codei);
					}
				}
				Operand func2 = GenerateOperandFunc(func1);
				InterCode ic3 = GenerateInterCodeCall(func2, op);
				InterCodes code3 = singleCode(ic2);
				codeAdd(code1, code2);
				codeAdd(code1, code3);
				return code1;
				
			}	
		default: { printf("Error in translate_Exp\n"); exit(0); }
	}
}

InterCodes translate_Args(Node node) {
	switch(node->childNum) {
		case 1: {
				char t1[20];
				new_temp(t1);
				InterCodes code1 = translate_Exp(node->child[0], t1);
				addArg(t1);	
				//arg_list = t1 + arg_list;
				return code1;
			}
		case 3: {
				char t1[20];
				new_temp(t1);
				InterCodes code1 = translate_Exp(node->child[0], t1);
				addArg(t1);
				//arg_list = t1 + arg_list;
				InterCodes code2 = translate_Args(node->child[2]);
				codeAdd(code1, code2);
				return code1;
			}
	}
}

InterCodes translate_Program(Node node) {
	//TODO need to finish
	InterCodes c1 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[0]);	
	InterCodes c2 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[0]);	
	InterCodes c3 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[1]->child[0]);	
	//InterCodes c4 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[1]->child[1]->child[0]);
	//InterCodes c5 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[1]->child[1]->child[1]->child[0]);
	//InterCodes c6 = translate_Stmt(node->child[0]->child[0]->child[2]->child[2]->child[1]->child[1]->child[1]->child[1]->child[1]->child[0]);
	
	return codeAdd(c1, codeAdd(c2, c3));//codeAdd(c3, c4)));//codeAdd(c4,codeAdd(c5,c6)))));
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
