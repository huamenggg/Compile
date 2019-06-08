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

Operand GenerateOperandCall(FuncList func) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = FUNCTION;
	op->u.func = func;
	return op;
}

Operand GenerateOperandArg(FieldList a) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = ARGUMENT;
	op->u.symbol = generateField(a->name, NULL);
	return op;
}

Operand GenerateOperandParam(FieldList f) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = PARAM;
	op->u.symbol = f;
	return op;
}

Operand GenerateOperandWrite(FieldList a) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = WRITE;
	op->u.symbol = a;
	return op;
}

Operand GenerateOperandGetAddress(FieldList f) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = GETADDRESS;
	op->u.symbol = f;
	return op;
}

Operand GenerateOperandGetValue(FieldList f) {
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = GETVALUE;
	op->u.symbol = f;
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

InterCode GenerateInterCodeFunc(Operand op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = FUNCTIONI;
	ic->u.value = op;
	return ic;
}

InterCode GenerateInterCodeArg(Operand op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = ARG;
	ic->u.value = op;
	return ic;
}

InterCode GenerateInterCodeParam(Operand op) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = PARAMI;
	ic->u.value = op;
	return ic;
}

InterCode GenerateInterCodeDec(Operand op, int size) {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = DECI;
	ic->u.dec.op = op;
	ic->u.dec.size = size;
	return ic;
}

InterCode GenerateInterCodeDivide() {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = DIVIDE;
	return ic;
}

InterCode GenerateInterCodeArgclear() {
	InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
	ic->kind = ARGCLEAR;
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
	int i;
	labelNum = 1;
	tempNum = 1;
	stackAddress = 0;
	for(i = 0;i < regNum;i++) {
		Reg newReg = (Reg)malloc(sizeof(struct Reg_));
		regs[i] = newReg;
		sprintf(regs[i]->name, "$t%d", i);
		regs[i]->status = AVAILABLE;
	}
	for(i = 0;i < argNum;i++) {
		Reg newArg = (Reg)malloc(sizeof(struct Reg_));
		args[i] = newArg;
		sprintf(args[i]->name, "$a%d", i);
		args[i]->status = AVAILABLE;
	}
}

int DeleteInterCodes(InterCodes del) {
	del->pre->next = del->next;
	del->next->pre = del->pre;
	free(del);
	return 1;
}

void writeOperand(Operand op, FILE* f) {
	if(op->kind == VARIABLE) {
		printf("%s", op->u.symbol->name);
		fprintf(f, "%s", op->u.symbol->name);
	}
	else if(op->kind == TEMPORLABEL) {
		printf("%s", op->u.name);
		fprintf(f, "%s", op->u.name);
	}
	else if(op->kind == BADD || op->kind == BMINUS || op->kind == BSTAR || op->kind == BDIV) {
		Operand op1 = op->u.bi.a;
		Operand op2 = op->u.bi.b;
		writeOperand(op1, f);
		if(op->kind == BADD) {
			printf(" + ");
			fprintf(f, " + ");
		}
		if(op->kind == BMINUS) {
			printf(" - ");
			fprintf(f, " - ");
		}
		if(op->kind == BSTAR) {
			printf(" * ");
			fprintf(f, " * ");
		}
		if(op->kind == BDIV) {
			printf(" / ");
			fprintf(f, " / ");
		}
		writeOperand(op2, f);
	}
	else if(op->kind == CONSTANT) {
		printf("#%d", op->u.value);
		fprintf(f, "#%d", op->u.value);
	}
	else if(op->kind == RE) {
		printf("%s", op->u.name);
		fprintf(f, "%s", op->u.name);
	}
	else if(op->kind == PARAM) {
		printf("%s", op->u.symbol->name);
		fprintf(f, "%s", op->u.symbol->name);
	}
	else if(op->kind == CALL) {
		printf("CALL %s", op->u.func->name);
		fprintf(f, "CALL %s", op->u.func->name);
	}
	else if(op->kind == ARGUMENT || op->kind == WRITE) {
		printf("%s", op->u.symbol->name);
		fprintf(f, "%s", op->u.symbol->name);
	}
	else if(op->kind == GETADDRESS) {
		printf("&%s", op->u.symbol->name);
		fprintf(f, "&%s", op->u.symbol->name);
	}
	else if(op->kind == GETVALUE) {
		printf("*%s", op->u.symbol->name);
		fprintf(f, "*%s", op->u.symbol->name);
	}
	else if(op->kind == FUNCTION) {
		printf("%s", op->u.func->name);
		fprintf(f, "%s", op->u.func->name);
	}
	else {
		printf("%d", op->u.value);
		fprintf(f, "%d", op->u.value);
	}
}

void preWrite(FILE *f) {
	fprintf(f, ".globl main\n");
	fprintf(f, ".data\n");
	fprintf(f, "  _prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(f, "  _ret: .asciiz \"\\n\"\n\n");
	//TODO global data need to finish
	fprintf(f, ".text\n");
	fprintf(f, "write:\n");
	fprintf(f, "  li $v0, 1\n");
	fprintf(f, "  syscall\n");
	fprintf(f, "  li $v0, 4\n");
	fprintf(f, "  la $a0, _ret\n");
	fprintf(f, "  syscall\n");
	fprintf(f, "  move $v0, $0\n");
	fprintf(f, "  jr $ra\n\n");
	fprintf(f, "read:\n");
	fprintf(f, "  li $v0, 4\n");
	fprintf(f, "  la $a0, _prompt\n");
	fprintf(f, "  syscall\n");
	fprintf(f, "  li $v0, 5\n");
	fprintf(f, "  syscall\n");
	fprintf(f, "  jr $ra\n");
}

int getReg(Operand op, FILE *f) {
	/*printf("reg status:");
	for(int i = 0;i < regNum;i++) {
		printf("%d ", regs[i]->status);
	}
	printf("\n");*/
	int i;
	if(op->kind == TEMPORLABEL) {
		//printf("%s\n", op->u.name);
		for(i = 0;i < regNum;i++) {
			if(regs[i]->status == LOCKED && regs[i]->op 
					&& regs[i]->op->kind == TEMPORLABEL 
					&& strcmp(regs[i]->op->u.name, op->u.name) == 0) {
				return i;
			}
		}
		for(i = 0;i < regNum;i++) {
			if(regs[i]->status == AVAILABLE) {
				regs[i]->status = LOCKED;
				regs[i]->op = op;
				//printf("regs[%d]:%s\n", i, regs[i]->op->u.name);
				return i;
			}
		}
		printf("No available register error\n");
		exit(0);
	}
	else if(op->kind == WRITE || op->kind == ARGUMENT) {
		for(i = 0;i < regNum;i++) {
			//printf("Write:regs[%d]:%s\n", i, regs[i]->op->u.name);
			if(regs[i]->op && regs[i]->op->kind == TEMPORLABEL &&
					strcmp(regs[i]->op->u.name, op->u.symbol->name) == 0) {
				return i;
			}
		}
		printf("Error: write or argument use undefined temp variable\n");
		exit(0);
	}
	else {
		printf("Error in get register\n");
		exit(0);
	}
}

void storeAllReg(FILE *f) {
	fprintf(f, "  addi $sp, $sp, -%d\n", 4 * regNum);
	for(int i = 0;i < regNum;i++) {
		fprintf(f, "  sw %s, %d($sp)\n", regs[i]->name, i * 4);
	}
}

void recallAllReg(FILE *f) {
	for(int i = 0;i < regNum;i++) {
		fprintf(f, "  lw %s, %d($sp)\n", regs[i]->name, i * 4);
	}
	fprintf(f, "  addi $sp, $sp, %d\n", 4 * regNum);
}

void writeToFile(FILE *f) {
#ifndef INTER
	preWrite(f);
#endif
	InterCodes out = head;
	while(out != NULL) {
		InterCode ic = out->code;
		if(ic->kind == ASSIGNI) {
			Operand op1 = ic->u.assign.left;
			Operand op2 = ic->u.assign.right;
#ifdef INTER
			writeOperand(op1, f);
			printf(" := ");
			fprintf(f, " := ");
			writeOperand(op2, f);	
			printf("\n");
			fprintf(f, "\n");
#else
			//if op1 is *t1
			if(op1->kind == TEMPORLABEL && op1->u.name[0] == '*') {
				int reg1, reg2, i;
				if(op2->u.name[0] == '*') {
					Operand op = GenerateOperandTemp("_temp_1");
					reg2 = getReg(op, f);
					for(i = 0;op2->u.name[i] != '\0';i++) {
						op2->u.name[i] = op2->u.name[i + 1];
					}
					int reg3 = getReg(op2, f);
					fprintf(f, "  lw %s, (%s)\n", regs[reg2]->name, regs[reg3]->name);
					regs[reg3]->status = AVAILABLE;
				}
				else {
					reg2 = getReg(op2, f);
				}
				for(i = 0;op1->u.name[i] != '\0';i++) {
					op1->u.name[i] = op1->u.name[i + 1];
				}
				reg1 = getReg(op1, f);
				fprintf(f, "  sw %s, (%s)\n", regs[reg2]->name, regs[reg1]->name);
				regs[reg1]->status = AVAILABLE;
				regs[reg2]->status = AVAILABLE;
			}
			else if(op1->kind == VARIABLE) {
				if(op2->kind != TEMPORLABEL) {
					printf("Error: variable := not temp\n");
					exit(0);
				}
				int reg2;
				if(op2->u.name[0] == '*') {
					Operand op = GenerateOperandTemp("_temp_");
					reg2 = getReg(op, f);
					for(int i = 0;op2->u.name[i] != '\0';i++) {
						op2->u.name[i] = op2->u.name[i + 1];
					}
					int reg3 = getReg(op2, f);
					fprintf(f, "  lw %s, (%s)\n", regs[reg2]->name, regs[reg3]->name);
					regs[reg3]->status = AVAILABLE;
				}
				else {
					reg2 = getReg(op2, f);
				}
				fprintf(f, "  sw %s, %d($sp)\n", regs[reg2]->name, (stackAddress - op1->u.symbol->stackIndex));
				regs[reg2]->status = AVAILABLE;
			}
			else if(op2->kind == CONSTANT) {
				if(op1->kind != TEMPORLABEL) {
					printf("Error: not temp := Constant\n");
					exit(0);
				}
				int reg = getReg(op1, f);
				fprintf(f, "  li %s, %d\n", regs[reg]->name, op2->u.value);
			}
			else if(op2->kind == TEMPORLABEL) {
				if(op1->kind != TEMPORLABEL) {
					printf("Error: not variable or temp := temp\n");
					exit(0);
				}
				int reg1 = getReg(op1, f);
				int reg2;
				if(op2->u.name[0] == '*') {
					Operand op = GenerateOperandTemp("_temp_");
					reg2 = getReg(op, f);
					for(int i = 0;op2->u.name[i] != '\0';i++) {
						op2->u.name[i] = op2->u.name[i + 1];
					}
					int reg3 = getReg(op2, f);
					fprintf(f, "  lw %s, (%s)\n", regs[reg2]->name, regs[reg3]->name);
					regs[reg3]->status = AVAILABLE;
				}
				else {
					reg2 = getReg(op2, f);
				}
				fprintf(f, "  move %s, %s\n", regs[reg1]->name, regs[reg2]->name);
				regs[reg2]->status = AVAILABLE;
			}
			else if(op2->kind == VARIABLE) {
				int reg = getReg(op1, f);
				//printf("op2 variable:stackAddress:%d\n", stackAddress);
				fprintf(f, "  lw %s, %d($sp)\n", regs[reg]->name, (stackAddress - op2->u.symbol->stackIndex));
			}
			else if(op2->kind == BADD || op2->kind == BMINUS || op2->kind == BSTAR || op2->kind == BDIV) {
				int reg1;
				int reg2;
				int reg3;
				if(op1->kind != TEMPORLABEL) {
					printf("Error in not temp := a + b\n");
					exit(0);
				}
				reg1 = getReg(op1, f);
				
				if(op2->u.bi.b->kind == CONSTANT) {
					Operand op = GenerateOperandTemp("_temp_");
					reg3 = getReg(op, f);
					fprintf(f, "  li %s, %d\n", regs[reg3]->name, op2->u.bi.b->u.value);
					if(op2->u.bi.a->kind != TEMPORLABEL) {
						printf("Error: the first operation of +-*/ isn't temp when the second is CONSTANT\n");
						exit(0);
					}
					reg2 = getReg(op2->u.bi.a, f);
				}
				else if(op2->u.bi.a->kind == CONSTANT) {
					Operand op = GenerateOperandTemp("_temp_");
					reg2 = getReg(op, f);
					fprintf(f, "  li %s, %d\n", regs[reg2]->name, op2->u.bi.a->u.value);
					if(op2->u.bi.b->kind != TEMPORLABEL) {
						printf("Error: the second operation of +-*/ isn't temp when the first is CONSTANT\n");
						exit(0);
					}
					reg3 = getReg(op2->u.bi.b, f);
				}

				else if(op2->u.bi.a->kind == GETADDRESS) {
					Operand op = GenerateOperandTemp("_temp_");
					reg2 = getReg(op, f);
					fprintf(f, "  la %s, %d($sp)\n", regs[reg2]->name, (stackAddress - op2->u.bi.a->u.symbol->stackIndex));
					if(op2->u.bi.b->kind != TEMPORLABEL) {
						printf("Error: the second operation of +-*/ isn't temp when the first is ADDRESS\n");
						exit(0);
					}
					reg3 = getReg(op2->u.bi.b, f);
				}			
				else if(op2->u.bi.b->kind != TEMPORLABEL || op2->u.bi.a->kind != TEMPORLABEL) {
					printf("Error: first or second operation of +,-,*,/ isn't temp\n");
					exit(0);
				}
				else {
					reg2 = getReg(op2->u.bi.a, f);
					reg3 = getReg(op2->u.bi.b, f);
				}
	
				if(op2->kind == BADD)
					fprintf(f, "  add %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, regs[reg3]->name);
				else if(op2->kind == BMINUS)
					fprintf(f, "  sub %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, regs[reg3]->name);
				else if(op2->kind == BSTAR)
					fprintf(f, "  mul %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, regs[reg3]->name);
				else {
					fprintf(f, "  div %s, %s\n", regs[reg2]->name, regs[reg3]->name);
					fprintf(f, "  mflo %s\n", regs[reg1]->name);
				}
				regs[reg2]->status = AVAILABLE;
				regs[reg3]->status = AVAILABLE;
			}
			else if(op2->kind == GETADDRESS) {
				printf("Error: op2->kind shouldn't be GETADDRESS\n");
				exit(0);
			}
			else {
				printf("Error default\n");
				exit(0);
			}
#endif
		}
		else if(ic->kind == RETURNI) {
#ifdef INTER
			printf("RETURN ");
			fprintf(f, "RETURN ");
			writeOperand(ic->u.value, f);
			printf("\n");
			fprintf(f, "\n");
#else
			int reg = getReg(ic->u.value, f);
			fprintf(f, "  move $v0, %s\n", regs[reg]->name);
			fprintf(f, "  jr $ra\n");
			regs[reg]->status = AVAILABLE;
#endif
		}
		else if(ic->kind == COND1) {
			Operand t1 = ic->u.cond1.t1;
			Operand op = ic->u.cond1.op;
			Operand t2 = ic->u.cond1.t2;
			Operand label = ic->u.cond1.label;		
#ifdef INTER
			printf("IF ");
			fprintf(f, "IF ");
			writeOperand(t1, f);
			printf(" ");
			fprintf(f, " ");
			writeOperand(op, f);
			printf(" ");
			fprintf(f, " ");
			writeOperand(t2, f);
			printf(" GOTO ");
			fprintf(f, " GOTO ");
			writeOperand(label, f);
			printf("\n");
			fprintf(f, "\n");
#else
			int reg1, reg2;
			//printf("t1:%s t2:%s\n", t1->u.name, t2->u.name);
			if(t1->kind == TEMPORLABEL && t1->u.name[0] == '*') {
				Operand op = GenerateOperandTemp("_temp1_");
				reg1 = getReg(op, f);
				for(int i = 0;t1->u.name[i] != '\0';i++) {
					t1->u.name[i] = t1->u.name[i + 1];
				}
				int reg3 = getReg(t1, f);
				fprintf(f, "  lw %s, (%s)\n", regs[reg1]->name, regs[reg3]->name);
				regs[reg3]->status = AVAILABLE;
			}
			else {
				reg1 = getReg(t1, f);
			}
			if(t2->kind == TEMPORLABEL && t2->u.name[0] == '*') {
				Operand op = GenerateOperandTemp("_temp2_");
				reg2 = getReg(op, f);
				for(int i = 0;t2->u.name[i] != '\0';i++) {
					t2->u.name[i] = t2->u.name[i + 1];
				}
				int reg3 = getReg(t2, f);
				fprintf(f, "  lw %s, (%s)\n", regs[reg2]->name, regs[reg3]->name);
				regs[reg3]->status = AVAILABLE;
			}
			else {
				reg2 = getReg(t2, f);
			}
			if(strcmp(op->u.name, "==") == 0) {
				fprintf(f, "  beq %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, label->u.name);
			}
			else if(strcmp(op->u.name, "!=") == 0) {
				fprintf(f, "  bne %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, label->u.name);
			}
			else if(strcmp(op->u.name, ">") == 0) {
				fprintf(f, "  bgt %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, label->u.name);
			}
			else if(strcmp(op->u.name, "<") == 0) {
				fprintf(f, "  blt %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, label->u.name);
			}
			else if(strcmp(op->u.name, ">=") == 0) {
				fprintf(f, "  bge %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, label->u.name);
			}
			else if(strcmp(op->u.name, "<=") == 0) {
				fprintf(f, "  ble %s, %s, %s\n", regs[reg1]->name, regs[reg2]->name, label->u.name);
			}
			regs[reg1]->status = AVAILABLE;
			regs[reg2]->status = AVAILABLE;
#endif
		}
		else if(ic->kind == GOTO) {
#ifdef INTER
			printf("GOTO ");
			fprintf(f, "GOTO ");
			writeOperand(ic->u.value, f);
			printf("\n");
			fprintf(f, "\n");
#else
			fprintf(f, "  j %s\n", ic->u.value->u.name);
#endif
		}
		else if(ic->kind == LABEL) {
#ifdef INTER
			printf("LABEL ");
			fprintf(f, "LABEL ");
			writeOperand(ic->u.value, f);
			printf(" :\n");
			fprintf(f, " :\n");
#else
			fprintf(f, "%s:\n", ic->u.value->u.name);
#endif
		}
		else if(ic->kind == READI) {
#ifdef INTER
			printf("READ ");
			fprintf(f, "READ ");
			writeOperand(ic->u.value, f);
			printf("\n");
			fprintf(f, "\n");
#else
			int reg = getReg(ic->u.value, f);
			storeAllReg(f);
			fprintf(f, "  addi $sp, $sp, -4\n");
			fprintf(f, "  sw $ra, 0($sp)\n");
			fprintf(f, "  jal read\n");
			fprintf(f, "  lw $ra, 0($sp)\n");
			fprintf(f, "  addi $sp, $sp, 4\n");
			recallAllReg(f);
			fprintf(f, "  move %s, $v0\n", regs[reg]->name);
#endif
		}
		else if(ic->kind == CALLI) {
			Operand func = ic->u.call.func;
			Operand place = ic->u.call.place;
#ifdef INTER
			if(place) {
				writeOperand(place, f);
			}
			else {
				printf("_temp_return");
				fprintf(f, "_temp_return");
			}
			printf(" := ");
			fprintf(f, " := ");
			printf("CALL %s\n", func->u.func->name);
			fprintf(f, "CALL %s\n", func->u.func->name);
#else
			storeAllReg(f);
			fprintf(f, "  addi $sp, $sp, -4\n");
			fprintf(f, "  sw $ra, 0($sp)\n");
			fprintf(f, "  addi $sp, $sp, -4\n");
			//TODO: might have error
			fprintf(f, "  sw $fp, 0($sp)\n");
			fprintf(f, "  move $fp, $sp\n");
			fprintf(f, "  jal %s\n", func->u.func->name);
			fprintf(f, "  move $sp, $fp\n");
			fprintf(f, "  lw $fp, 0($sp)\n");
			fprintf(f, "  addi $sp, $sp, 4\n");
			fprintf(f, "  lw $ra, 0($sp)\n");
			fprintf(f, "  addi $sp, $sp, 4\n");
			recallAllReg(f);
			if(place) {
				if(place->kind != TEMPORLABEL) {
					printf("Error: not temp := CALL function\n");
					exit(0);
				}
				int reg;
				if(place->u.name[0] == '*') {
					Operand op = GenerateOperandTemp("_temp_");
					int regtemp = getReg(op, f);
					for(int i = 0;place->u.name[i] != '\0';i++) {
						place->u.name[i] = place->u.name[i + 1];
					}
					reg = getReg(place, f);
					fprintf(f, "  move %s, $v0\n", regs[regtemp]->name);
					fprintf(f, "  sw %s, (%s)\n", regs[regtemp]->name, regs[reg]->name);
					regs[regtemp]->status = AVAILABLE;
				}
				else {
					reg = getReg(place, f);
					fprintf(f, "  move %s, $v0\n", regs[reg]->name);
				}
			}
#endif
		}
		else if(ic->kind == WRITEI) {
#ifdef INTER
			printf("WRITE ");
			fprintf(f, "WRITE ");
			writeOperand(ic->u.value, f);
			printf("\n");
			fprintf(f, "\n");
#else
			int reg;
			if(ic->u.value->u.symbol->name[0] == '*') {
				Operand op = GenerateOperandTemp("_temp_");
				reg = getReg(op, f);
				int reg1;
				for(int i = 0;ic->u.value->u.symbol->name[i] != '\0';i++) {
					ic->u.value->u.symbol->name[i] = ic->u.value->u.symbol->name[i + 1];
				}
				reg1 = getReg(ic->u.value, f);
				fprintf(f, "  lw %s, (%s)\n", regs[reg]->name, regs[reg1]->name);
				regs[reg1]->status = AVAILABLE;
			}
			else {
				reg = getReg(ic->u.value, f);
			}
			fprintf(f, "  move $a0, %s\n", regs[reg]->name);
			storeAllReg(f);
			fprintf(f, "  addi $sp, $sp, -4\n");
			fprintf(f, "  sw $ra, 0($sp)\n");
			fprintf(f, "  jal write\n");
			fprintf(f, "  lw $ra, 0($sp)\n");
			fprintf(f, "  addi $sp, $sp, 4\n");
			recallAllReg(f);
			regs[reg]->status = AVAILABLE;

#endif
		}
		else if(ic->kind == ARG) {
#ifdef INTER
			printf("ARG ");
			fprintf(f, "ARG ");
			writeOperand(ic->u.value, f);
			printf("\n");
			fprintf(f, "\n");
#else
			int i, arg;
			for(i = 0;i < argNum;i++) {
				if(args[i]->status == AVAILABLE) {
					//printf("args:%d locked\n", i);
					arg = i;
					args[i]->status = LOCKED;
					break;
				}
			}
			if(ic->u.value->u.symbol->name[0] == '*') {
				for(i = 0;ic->u.value->u.symbol->name[i] != '\0';i++) {
					ic->u.value->u.symbol->name[i] = ic->u.value->u.symbol->name[i + 1];
				}
				int reg = getReg(ic->u.value, f);
				Operand op = GenerateOperandTemp("_temp_");
				int regtemp = getReg(op, f);
				fprintf(f, "  lw %s, (%s)\n", regs[regtemp]->name, regs[reg]->name);
				regs[reg]->status = AVAILABLE;
				fprintf(f, "  move %s, %s\n", args[arg]->name, regs[regtemp]->name);
				regs[regtemp]->status = AVAILABLE;
			}
			else {
				int reg = getReg(ic->u.value, f);
				fprintf(f, "  move %s, %s\n", args[arg]->name, regs[reg]->name);
				regs[reg]->status = AVAILABLE;
			}
			/*printf("ARG:");
			for(i = 0;i < argNum;i++) {
				printf("%d ", args[i]->status);
			}
			printf("\n");*/
#endif
		}
		else if(ic->kind == DECI) {
#ifdef INTER
			printf("DEC ");
			fprintf(f, "DEC ");
			writeOperand(ic->u.dec.op, f);
			printf(" %d\n", ic->u.dec.size);
			fprintf(f, " %d\n", ic->u.dec.size);
#else
			fprintf(f, "  addi $sp, $sp, -%d\n", ic->u.dec.size);
			stackAddress += ic->u.dec.size;
			ic->u.dec.op->u.symbol->stackIndex = stackAddress;
			//printf("DEC:stackAddress:%d\n", stackAddress);
#endif
		}
		else if(ic->kind == PARAMI) {
#ifdef INTER
			printf("PARAM ");
			fprintf(f, "PARAM ");
			writeOperand(ic->u.value, f);
			printf("\n");
			fprintf(f, "\n");
#else
			fprintf(f, "  addi $sp, $sp, -4\n");
			stackAddress += 4;
			ic->u.value->u.symbol->stackIndex = stackAddress;
			int i, arg;
			/*printf("arg status:");
			for(i = 0;i < argNum;i++) {
				printf("%d ", args[i]->status);
			}
			printf("\n");*/
			for(i = 0;i < argNum;i++) {
				if(args[i]->status == AVAILABLE) {
					arg = i;
					args[i]->status = LOCKED;
					break;
				}
			}
			fprintf(f, "  sw %s, %d($sp)\n", args[arg]->name,
					(stackAddress - ic->u.value->u.symbol->stackIndex));
#endif
		}
		else if(ic->kind == FUNCTIONI) {
#ifdef INTER
			printf("FUNCTION ");
			fprintf(f, "FUNCTION ");
			writeOperand(ic->u.value, f);
			printf(" :\n");
			fprintf(f, " :\n");
#else
			stackAddress = 0;
			fprintf(f, "\n%s:\n", ic->u.value->u.func->name);
#endif
		}
		/*else if(ic->kind == DIVIDE) {
#ifdef INTER
			printf("DIVIDE\n");
#else
			//printf("stack goto 0\n");
			fprintf(f, "  addi $sp, $sp, %d\n", stackAddress);
			stackAddress = 0;
#endif
		}*/
		else if(ic->kind == ARGCLEAR) {
#ifdef INTER
			printf("ARGCLEAR\n");
#else
			for(int i = 0;i < argNum;i++) {
				args[i]->status = AVAILABLE;
			}
#endif
		}
		out = out->next;
	}
	return;
}

InterCodes translate_Stmt(Node node) {
	//printf("Stmt\n");
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1: { /* Stmt -> CompSt */
				return translate_CompSt(node->child[0]);
			}
		case 2: { /* Stmt -> Exp SEMI */
				return translate_Exp(node->child[0], NULL, 0);
			}
		case 3: { /* Stmt -> RETURN Exp SEMI */
				char t1[20];
				new_temp(t1);
				//TODO fieldlist delete repeat
				InterCodes code1 = translate_Exp(node->child[1], t1, 0);
				Operand op = GenerateOperandTemp(t1);
				InterCode ic = GenerateInterCodeReturnOrLabel(RETURNI, op);	
				InterCodes code2 = singleCode(ic);
				return codeAdd(code1, code2);
				/*InterCode divideCode = GenerateInterCodeDivide();
				InterCodes code3 = singleCode(divideCode);
				return codeAdd(code1, codeAdd(code3, code2));*/
			}
		case 5: {
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
	return NULL;
}

InterCodes translate_Cond(Node node, char* label1, char* label2) {
	//printf("Cond\n");
	if(node == NULL)
		return NULL;	
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
					InterCodes code1 = translate_Exp(node->child[0], t1, 0);
					InterCodes code2 = translate_Exp(node->child[2], t2, 0);
					Operand op1 = GenerateOperandTemp(t1);
					Operand op2 = GenerateOperandTemp(t2);
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
				InterCodes code1 = translate_Exp(node, t1, 0);
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
	return NULL;
}

InterCodes translate_Exp(Node node, char* place, int ifGetAddress) {
	//printf("Exp%s", place);
	Operand op = GenerateOperandTemp(place);
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1: {
				if(strcmp(node->child[0]->name, "ID") == 0) {
					if(op) {
						//Different from case3
						//this rule is Exp->ID
						//so node->child[0] is ID
						FieldList f = getSymbol(node->child[0]->stringValue);
						Operand op1;
						if(ifGetAddress)
							op1 = GenerateOperandGetAddress(f);
						else
							op1 = GenerateOperandVariable(f);
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
				if(strcmp(node->child[0]->name, "MINUS") == 0) {
					char t1[20];
					new_temp(t1);
					InterCodes code1 = translate_Exp(node->child[1], t1, 0);
					Operand op1 = GenerateOperand(CONSTANT, 0);
					Operand op2 = GenerateOperandTemp(t1);
					Operand op3 = op2 = GenerateOperandBi(BMINUS, op1, op2);
					InterCode ic = GenerateInterCodeAssign(op3, op);
					InterCodes code2 = singleCode(ic);
					return codeAdd(code1, code2);
				}
				else {
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
			}
		case 3: {
				if(strcmp(node->child[0]->name, "LP") == 0) {
					return translate_Exp(node->child[1], place, ifGetAddress);
				}
				else if(strcmp(node->child[1]->name, "ASSIGNOP") == 0) {
					//printf("ASSIGNOP\n");
					// this rule is Exp->Exp = Exp
					// so node is Exp(left), Exp->child[0] is Exp(right)
					// but what we need is Exp->ID
					// we need ID->stringValue
					// so need node->child[0]->child[0]
					InterCodes code1 = NULL;
					Operand op1;
					//printf("%d\n", node->child[0]->childNum);
					if(node->child[0]->childNum > 1) { 
						if(strcmp(node->child[0]->child[1]->name, "LB") == 0
								|| strcmp(node->child[0]->child[1]->name, "DOT") == 0) {
							char t1[20];
							new_temp(t1);
							code1 = translate_Exp(node->child[0], t1, 0);
							op1 = GenerateOperandTemp(t1);
						}
					}
					else {
						FieldList f = getSymbol(node->child[0]->child[0]->stringValue);
						op1 = GenerateOperandVariable(f);
					}
					char t2[20];
					new_temp(t2);
					InterCodes code2 = translate_Exp(node->child[2], t2, 0);
					Operand op2 = GenerateOperandTemp(t2);
					InterCode ic = GenerateInterCodeAssign(op2, op1);
					InterCodes code3 = singleCode(ic);
					if(op) {
						InterCode ic1 = GenerateInterCodeAssign(op1, op);
						InterCodes code4 = singleCode(ic1);
						codeAdd(code3, code4);
					}
					if(code1) 
						return codeAdd(code1, codeAdd(code2, code3));
					
					else 
						return codeAdd(code2, code3);
					
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
					InterCodes code1 = translate_Exp(node->child[0], t1, 0);
					InterCodes code2 = translate_Exp(node->child[2], t2, 0);
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
					codeAdd(code1, code2);
					if(op) {
						InterCode ic1 = GenerateInterCodeAssign(op3, op);
						InterCodes code3 = singleCode(ic1);
						codeAdd(code1, code3);
					}
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
					Operand func2 = GenerateOperandCall(func1);
					InterCode ic2 = GenerateInterCodeCall(func2, op);
					InterCodes code2 = singleCode(ic2);
				       	return code2;
				}
				//TODO Exp DOT ID
				else if(strcmp(node->child[1]->name, "DOT") == 0) {
					InterCodes code1 = NULL;
					char t1[20];
					new_temp(t1);
					FieldList f;
					if(strcmp(node->child[0]->child[0]->name, "ID") == 0) {
						f = getSymbol(node->child[0]->child[0]->stringValue);
					}
					else {
						code1 = translate_Exp(node->child[0], t1, 0);
						if(t1[0] == '*') {
							for(int index = 0;t1[index] != '\0';index++)
								t1[index] = t1[index+1];
						}
						f = getSymbol(node->child[0]->child[2]->stringValue);
					}
		
					//o1
					FieldList f1 = f->type->u.structure;
					int addr = 0;
					for(; f1; f1 = f1->next) {
						if(strcmp(f1->name, node->child[2]->stringValue) == 0)
							break;
						if(f1->type->kind == BASIC) {
							addr += 1;
						}
						else if(f1->type->kind == ARRAY) {
							addr += f1->type->u.array.size;
						}
						else {
							addr += CalcStructSize(f1->type->u.structure);	
						}
					}
					//printf("addr:%d\n", addr);
					Operand opaddress;
					if(code1) {
						opaddress = GenerateOperandTemp(t1);
					}
					else {
						if(f->var == NPAVAR)
							opaddress = GenerateOperandGetAddress(f);
						else
							opaddress = GenerateOperandVariable(f);
					}
					Operand op3 = GenerateOperand(CONSTANT, addr*4);
					Operand op4 = GenerateOperandBi(BADD, opaddress, op3);
					char t3[20];
					new_temp(t3);
					Operand op5 = GenerateOperandTemp(t3);
					InterCode ic2 = GenerateInterCodeAssign(op4, op5);
					InterCodes code3 = singleCode(ic2);
					if(op) {
						if(ifGetAddress)
							strcpy(place, t3);
						else 
							sprintf(place, "*%s", t3);
					}
					return codeAdd(code1, code3);
				}
			}
		case 4: {
				if(strcmp(node->child[0]->name, "ID") == 0) {
					FuncList func1 = getFuncAddress(node->child[0]->stringValue);
					FieldList tempArgList = copyArgList();
					clearArgList();
					InterCode argclear = GenerateInterCodeArgclear();
					InterCodes argCode = singleCode(argclear);
					InterCodes argCode2 = singleCode(argclear);
					char t1[20];
					new_temp(t1);
					InterCodes code1 = translate_Args(node->child[2], t1, func1);
					if(strcmp(func1->name, "write") == 0) {
						Operand opa1 = GenerateOperandWrite(argList[0]);
						InterCode ic1 = GenerateInterCodeReadOrWrite(WRITEI, opa1);
						InterCodes code2 = singleCode(ic1);
						codeAdd(code1, code2);
						resetArgList(tempArgList);
						return code1;
					}
					if(argLength == 0) {
						printf("Error in translate_Exp should contain paramter\n");
						exit(0);
					}
					
					//if(argList[0]->type != NULL)
					//	printf("%d\n", argList[0]->type->kind);	
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
					Operand func2 = GenerateOperandCall(func1);
					InterCode ic3 = GenerateInterCodeCall(func2, op);
					InterCodes code3 = singleCode(ic3);
					resetArgList(tempArgList);
					//return codeAdd(code1, codeAdd(code2, code3));
					return codeAdd(argCode, codeAdd(code1, codeAdd(code2, codeAdd(argCode2, code3))));
				}
				else if(strcmp(node->child[1]->name, "LB") == 0){
					// don't need to implement mutiple diemnsion array, so node->child[0] must be Exp->ID
					if(strcmp(node->child[0]->child[0]->name, "ID") && 
							(node->child[0]->childNum > 1 && strcmp(node->child[0]->child[1]->name, "DOT"))) {
						printf("Can't support multiple dimension array.\n");
						exit(0);
					}
					Operand addr;
					InterCodes code0 = NULL;
					if(strcmp(node->child[0]->child[0]->name, "ID") == 0) {
						FieldList f = getSymbol(node->child[0]->child[0]->stringValue);
						addr = GenerateOperandGetAddress(f);
					}
					else {
						char t0[20];
						new_temp(t0);
						code0 = translate_Exp(node->child[0], t0, 0);
						if(t0[0] == '*') {
							for(int index = 0;t0[index] != '\0';index++)
								t0[index] = t0[index+1];
						}
						addr = GenerateOperandTemp(t0);
					}
					char t1[20];
					new_temp(t1);
					InterCodes code1 = translate_Exp(node->child[2], t1, 0);
					char t2[20];
					new_temp(t2);
					Operand op1 = GenerateOperandTemp(t1);
					Operand op2 = GenerateOperandTemp(t2);
					Operand op3 = GenerateOperand(CONSTANT, 4);
					Operand op4 = GenerateOperandBi(BSTAR, op1, op3);
					InterCode ic1 = GenerateInterCodeAssign(op4, op2);
					InterCodes code2 = singleCode(ic1);
					char t3[20];
					new_temp(t3);
					Operand op5 = GenerateOperandTemp(t3);
					Operand op6 = GenerateOperandBi(BADD, addr, op2);
					InterCode ic2 = GenerateInterCodeAssign(op6, op5);
					InterCodes code3 = singleCode(ic2);
					if(op) {
						sprintf(place, "*%s", t3);
					}
					//printf("%x\n", code0);
					if(code0) return codeAdd(code0, codeAdd(code1, codeAdd(code2, code3)));
					return codeAdd(code1, codeAdd(code2, code3));
				}
			}	
		default: { printf("Error in translate_Exp\n"); exit(0); }
	}
	return NULL;
}

InterCodes translate_Args(Node node, char* place, FuncList f) {
	//printf("Args\n");
	if(node == NULL)
		return NULL;
	char temp[20];
	switch(node->childNum) {
		case 1: {
				InterCodes code1;
				if(f->parameters->type->kind == STRUCTURE) {
					code1 = translate_Exp(node->child[0], place, 1);
					sprintf(temp, "%s", place);
					strcpy(place, temp);
				}
				else
					code1 = translate_Exp(node->child[0], place, 0);
				addArg(place);
				return code1;
			}
		case 3: {
				//printf("I'm translate %s place:%s\n", node->child[0]->child[0]->stringValue, place);
				InterCodes code1;
				if(f->parameters->type->kind == STRUCTURE) {
					code1 = translate_Exp(node->child[0], place, 1);
					sprintf(temp, "%s", place);
					strcpy(place, temp);
				}
				else
					code1 = translate_Exp(node->child[0], place, 0);
				addArg(place);
				//arg_list = t1 + arg_list;
				char t1[20];
				new_temp(t1);
				InterCodes code2 = translate_Args(node->child[2], t1, f);
				return codeAdd(code1, code2);
			}
	}
	return NULL;
}

InterCodes translate_ExtDefList(Node node) {
	//printf("ExtDefList\n");
	if(node != NULL) {
		InterCodes code1 = translate_ExtDef(node->child[0]);
		InterCodes code2 = translate_ExtDefList(node->child[1]);
		return codeAdd(code1, code2);
	}
	return NULL;
}

InterCodes translate_ExtDef(Node node) {
	//printf("ExtDef\n");
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 2: {
				return NULL;
			}
		case 3: {
			if(strcmp(node->child[2]->name, "SEMI") == 0)
				return translate_ExtDecList(node->child[1]);
			else if(strcmp(node->child[2]->name, "CompSt") == 0) {
				InterCodes code1 = translate_FunDec(node->child[1]);
				InterCodes code2 = translate_CompSt(node->child[2]);
				return codeAdd(code1, code2);
			}
		}
	}
	return NULL;
}

InterCodes translate_ExtDecList(Node node) {
	//printf("ExtDecList\n");
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1: return translate_VarDec(node->child[0]);
		case 3: {
				InterCodes code1 = translate_VarDec(node->child[0]);
				InterCodes code2 = translate_ExtDecList(node->child[2]);
				return codeAdd(code1, code2);
			}
	}
	return NULL;
}

int CalcStructSize(FieldList f) {
	int size = 0;
	while(f != NULL) {
		if(f->type->kind == ARRAY) {
			size += f->type->u.array.size;
		}
		else if(f->type->kind == STRUCTURE) {
			size += CalcStructSize(f->type->u.structure);
		}
		else
			size++;
		f = f->next;
	}
	return size;
}

InterCodes translate_VarDec(Node node) {
	//printf("VarDec\n");
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1:	{
				FieldList f = getSymbol(node->child[0]->stringValue);
				if(f->type->kind == STRUCTURE) {
					//printf("VarDec struct\n");
					Operand op = GenerateOperandVariable(f);
					int size = 0;
					FieldList f1 = f->type->u.structure;
					//if(f1 != NULL)
					//	printf("!%s\n", f1->name);
					size = CalcStructSize(f1);
					size *= 4;
					InterCode ic = GenerateInterCodeDec(op, size);
					InterCodes code = singleCode(ic);
					return code;
				}
#ifdef INTER
				return NULL;
#else
				else {
					Operand op = GenerateOperandVariable(f);
					InterCode ic = GenerateInterCodeDec(op, 4);
					InterCodes code = singleCode(ic);
					return code;
				}
#endif
			}
		case 4: {
				if(strcmp(node->child[0]->child[0]->name, "ID")) {
					printf("Can't support multiple dimention array.\n");
					exit(0);
				}
				FieldList f = getSymbol(node->child[0]->child[0]->stringValue);
				Operand op = GenerateOperandVariable(f);
				int size = 4*(node->child[2]->numValue);
				InterCode ic = GenerateInterCodeDec(op, size);
				InterCodes code = singleCode(ic);
				return code;
			}
	}
	return NULL;
}

InterCodes translate_FunDec(Node node) {
	//printf("FunDec\n");
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 3: {
				Operand op = GenerateOperandFunction(getFuncAddress(node->child[0]->stringValue));
				InterCode ic = GenerateInterCodeFunc(op);
				InterCodes code = singleCode(ic);
				return code;
			}
		case 4: {
				Operand op = GenerateOperandFunction(getFuncAddress(node->child[0]->stringValue));
				InterCode ic = GenerateInterCodeFunc(op);
				InterCodes code1 = singleCode(ic);
				InterCode argclear = GenerateInterCodeArgclear(); 
				InterCodes argcode = singleCode(argclear);
				InterCodes argcode2 = singleCode(argclear);
				InterCodes code2 = translate_VarList(node->child[2]);
				//return codeAdd(code1, code2);
				return codeAdd(code1, codeAdd(argcode, codeAdd(code2, argcode2)));
			}
	}
	return NULL;
}

InterCodes translate_CompSt(Node node) {
	//printf("CompSt\n");
	if(node != NULL) {
		InterCodes code1 = translate_DefList(node->child[1]);
		InterCodes code2 = translate_StmtList(node->child[2]);
		return codeAdd(code1, code2);
	}
	return NULL;
}

InterCodes translate_VarList(Node node) {
	//printf("VarList\n");
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1: {
				if(!node->child[0]->child[1]->child[0]) {
				//VarList->Parameter->VarDec->ID
					printf("Error in reduction of VarDec -> ID\n");
					exit(0);
				}
				Operand op = GenerateOperandParam(getSymbol(node->child[0]->child[1]->child[0]->stringValue));
				InterCode ic = GenerateInterCodeParam(op);
				InterCodes code = singleCode(ic);
				return code;
			}
		case 3: {
				//FieldList f = getSymbol(node->child[0]->stringValue);
				//PARAM f
				Operand op1 = GenerateOperandParam(getSymbol(node->child[0]->child[1]->child[0]->stringValue));
				InterCode ic1 = GenerateInterCodeParam(op1);
				InterCodes code1 = singleCode(ic1);
				InterCodes code2 = translate_VarList(node->child[2]);
				return codeAdd(code1, code2);
			}
	}
	return NULL;
}

InterCodes translate_StmtList(Node node) {
	//printf("StmtList\n");
	if(node != NULL) {
		//childNum=2
		if(node->childNum == 2) {
			InterCodes code1 = translate_Stmt(node->child[0]);
			InterCodes code2 = translate_StmtList(node->child[1]);
			return codeAdd(code1, code2);
		}
	}
	return NULL;
}

InterCodes translate_DefList(Node node) {
	//printf("DefList\n");
	if(node != NULL) {
		InterCodes code1 = translate_Def(node->child[0]);
		InterCodes code2 = translate_DefList(node->child[1]);
		return codeAdd(code1, code2);
	}
	return NULL;
}

InterCodes translate_Def(Node node) {
	//printf("Def\n");
	if(node != NULL) {
		//printf("childNum:%d\n", node->childNum);
		return translate_DecList(node->child[1]);
	}
	return NULL;
}

InterCodes translate_DecList(Node node) {
	//printf("DecList childNum:%d\n", node->childNum);
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1: return translate_Dec(node->child[0]);
		case 3: {
				InterCodes code1 = translate_Dec(node->child[0]);
				InterCodes code2 = translate_DecList(node->child[2]);
				codeAdd(code1, code2);
				return code1;
			}
	}
}

InterCodes translate_Dec(Node node) {
	//printf("Dec childNum:%d\n", node->childNum);
	if(node == NULL)
		return NULL;
	switch(node->childNum) {
		case 1: return translate_VarDec(node->child[0]);
		case 3: {
				FieldList f = getSymbol(node->child[0]->child[0]->stringValue);
				Operand op1 = GenerateOperandVariable(f);
				char t1[20];
				new_temp(t1);
				Operand op2 = GenerateOperandTemp(t1);
				InterCodes code1 = translate_Exp(node->child[2], t1, 0);
				InterCode ic1 = GenerateInterCodeAssign(op2, op1);
				InterCodes code2 = singleCode(ic1);
#ifdef INTER
				return codeAdd(code1, code2);
#else
				InterCode icDec = GenerateInterCodeDec(op1, 4);
				InterCodes codeDec = singleCode(icDec);
				return codeAdd(codeDec, codeAdd(code1, code2));
#endif
			}
	}
}

InterCodes translate_Program(Node node) {
	//printf("Program\n");	
	InterCodes code = translate_ExtDefList(node->child[0]);
	return code;
}

void generateInterCode() {
	if(root == NULL) {
		printf("This is an empty syntax tree\n");
		return;
	}
	head = translate_Program(root);
	//Add
	if(head != NULL)
		tail = head->tail;
}

void new_temp(char* tempName) {
#ifndef DEBUG
	sprintf(tempName, "t__%dm", tempNum);
#else
	sprintf(tempName, "t%d", tempNum);
#endif
	tempNum++;
}

void new_label(char* label) {
	sprintf(label, "label%d", labelNum);
	labelNum++;
}

InterCodes codeAdd(InterCodes a, InterCodes b) {
	//printf("codeAdd\n");
	if(!b) return a;
	if(!a) {
		a = (InterCodes)malloc(sizeof(struct InterCodes_));
		a = b;
		return a;
	}
	a->tail->next = b;
	b->pre = a->tail;
	a->tail = b->tail;
	return a;
}
