#include"semantic.h"

void initSemantic() {
	/* initial typeList[0] to 'int' */
	Type t = (Type)malloc(sizeof(struct Type_));
	t->kind = BASIC;
	t->u.basic = 0;
	strcpy(t->name, "int");
	typeList[0] = t;

	/* initial typeList[1] to 'float' */
	t = (Type)malloc(sizeof(struct Type_));
	t->kind = BASIC;
	t->u.basic = 1;
	strcpy(t->name, "float");
	typeList[1] = t;

	typeLength = 2;
}
int IsId(char ch) {
	if(ch == '_') return 1;
	if(ch >= '0' && ch <= '9') return 1;
	if(ch >= 'a' && ch <= 'z') return 1;
	if(ch >= 'A' && ch <= 'Z') return 1;
	return 0;
}

char* Filter(char* string) {
	int i;
	for(i = 0;i < strlen(string);i++)
		if(IsId(string[i]) == 0) break;
	string[i] = '\0';
	return string;
}

int getHashIndex(char* string) {
	int val = 0, i;
	for(; *string; ++string) {
		val = (val << 2) + *string;
		if(i = val & ~hashLength)
			val = (val ^ (i >> 12)) & hashLength;
	}
	//printf("hash index:%d\n", val);
	return val;
}

Type getTypeAddress(char* typeName, int line, int ifPrint) {
	int i;
	if(strcmp(typeName, "int") == 0)
		return typeList[0];
	if(strcmp(typeName, "float") == 0)
		return typeList[1];
	for(i = 2; i < typeLength; i++) {
		//printf("typeName:%s\n", typeName);
		//printf("if equal:%s %s %d\n", typeName, typeList[i]->name, strcmp(typeName, typeList[i]->name));
		if(strcmp(typeName, typeList[i]->name) == 0) {
			//printf("I'm here %d\n", i);
			return typeList[i];
		}
	}
	if(ifPrint)
		printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", line, typeName);
	return NULL;
}

Type insertType(Type type, int line) {
	if(getTypeAddress(type->name, line, 1) == NULL) {
		typeList[typeLength] = type;
		typeLength++;
		return type;
	}
	printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", line, type->name);
	return type;
}

Type generateType(char* name, FieldList head) {
	Type t = (Type)malloc(sizeof(struct Type_));
	strcpy(t->name, name);
	t->kind = STRUCTURE;
	t->u.structure = head;
	return t;
}

Type generateTypeArray(int size) {
	Type t = (Type)malloc(sizeof(struct Type_));
	t->kind = ARRAY;
	t->u.array.size = size;
	t->u.array.elem = NULL;
	return t;
}

FieldList generateField(char* name, Type type) {
	FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
	strcpy(f->name, name);
	//printf("f->name:%s\n", f->name);
	f->type = type;
	f->next = NULL;
	return f;
}

FieldList insertSymbol(FieldList field, int line) {
	FieldList s = getSymbol(field->name);
	if(s == NULL) {
		int index = getHashIndex(field->name);
		s = (FieldList)malloc(sizeof(struct FieldList_));
		strcpy(s->name, field->name);
		s->type = field->type;
		s->next = symbolList[index];
		symbolList[index] = s;
		return s;
	}
	/* TODO:line */
	printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", line, field->name);
	return s;
}

FieldList getSymbol(char* name) {
	//mmm
	if(name == NULL)
		return NULL;

	int index = getHashIndex(name);
	//printf("name:%s\t", name);
	//printf("index:%d\n", index);
	FieldList s = symbolList[index];
	while(s != NULL) {
		if(strcmp(s->name, name) == 0)
			return s;
		s = s->next;
	}
	/* TODO:handle error of use undefined symbol */
	return NULL;
}

FuncList generateFunc(char* name, FieldList parameter, Type return_type) {
	FuncList f = (FuncList)malloc(sizeof(struct FuncList_));
	strcpy(f->name, name);
	//printf("f->name:%s\n", f->name);
	f->return_type = return_type;
	f->parameters = parameter;
	f->next = NULL;
	return f;
}

FuncList insertFunc(FuncList Func, int line) {
	FuncList f = getFuncAddress(Func->name);
	if(f == NULL) {
		int index = getHashIndex(Func->name);
		f = (FuncList)malloc(sizeof(struct FuncList_));
		strcpy(f->name, Func->name);
		f->return_type = Func->return_type;
		f->parameters = Func->parameters;
		f->next = funcList[index];
		f->line = line;
		f->status = Func->status;
		funcList[index] = f;
	}
	else if(f->status == DEC) {
		if(checkFuncEqual(f, Func, line, 0) == 1)
			f->status = Func->status;
		else
			printf("Error type ?? at Line %d: Function declaration mismatch with defination.\n", line);
	}
	else {
		printf("Error type 4 at Line %d: Redefined function \"%s\".\n", line, Func->name);
	}
	return f;

}

FuncList getFuncAddress(char* funName) {
	int index = getHashIndex(funName);
	//printf("name:%s\t", name);
	//printf("index:%d\n", index);
	FuncList f = funcList[index];
	while(f != NULL) {
		if(strcmp(f->name, funName) == 0)
			return f;
		f = f->next;
	}
	return NULL;
}

int checkFuncEqual(FuncList f1, FuncList f2, int line, int ifPrint) {
	if(typeEqual(f1->return_type, f2->return_type) == 0) {
		if(ifPrint)
			printf("Error type ?? at line %d: return type error\n", line);
		return 0;
	}
	return checkParameter(f1->name, f1->parameters, f2->parameters, line, ifPrint);	
}

void printType(Type t) {
	if(!t) return;
	if(t->kind == BASIC) printf("%s", t->name);
	else if(t->kind == STRUCTURE) printf("struct %s", t->name);
	else if(t->kind == ARRAY) {
		Type t1 = t;
		while(t1->kind == ARRAY) t1 = t1->u.array.elem;
		if(t1->kind == STRUCTURE)
			printf("struct ");
		printf("%s", t1->name);
		t1 = t;
		while(t1->kind == ARRAY) {
			printf("[%d]", t1->u.array.size);
			t1 = t1->u.array.elem;
		}
	}
}
void printParaType(FieldList t) {
	if(!t) return;
	while(t->next) {
		printType(t->type);
		printf(", ");	
		t = t->next;
	}
	printType(t->type);
}
int checkParameter(char* funcName, FieldList f1, FieldList f2, int line, int ifPrint) {
	FieldList t1 = f1, t2 = f2;
	while(f1 != NULL && f2 != NULL) {
		if(typeEqual(f1->type, f2->type) == 0) {
			if(ifPrint)
				printf("Error type ?? at line %d: mismatch parameters\n", line);
			return 0;
		}
		f1 = f1->next;
		f2 = f2->next;
	}
	if(f1 == NULL && f2 == NULL) return 1;
	if(ifPrint) {
		printf("Error type 9 at line %d: Function \"%s(", line, funcName);
		printParaType(t1);
		printf(")\" is not applicable for arguments \"(");
		printParaType(t2);
		printf(")\".\n");
	}
	return 0;
}

int typeEqual(Type t1, Type t2){
	if(!(t1 && t2))
		return 0;
	if(t1->kind == t2->kind){
		if(t1->kind == BASIC){
			if(t1->u.basic == t2->u.basic)
				return 1;
		}
		else if(t1->kind == ARRAY){
			if(typeEqual(t1->u.array.elem, t2->u.array.elem))
				return 1;
		}
		else if(t1->kind == STRUCTURE){
			if(t1->name == t2->name) return 1;
		}
	}
	return 0;
}

FieldList checkExp(FieldList exp1, FieldList exp2, char* error, int line){
	if(!exp1 || !exp2)
		return NULL;
	if(!strcmp(error,"ASSIGNOP")){
		if(strcmp(exp1->isLeftValue, "true") != 0) {
			printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", line);
			return NULL;
		}
		if(typeEqual(exp1->type, exp2->type) == 0){
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", line);
			return NULL;
		}
		return exp1;
	} 
	else if(!strcmp(error,"AND")||
		!strcmp(error,"OR")||
		!strcmp(error,"PLUS")||
		!strcmp(error,"MINUS")||
		!strcmp(error,"STAR")||
		!strcmp(error,"DIV")) {
		if(typeEqual(exp1->type, exp2->type) == 0){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n", line);
			return NULL;
		}
		if(exp1->type->kind != BASIC) {
			//TODO: non int or float participant in calc
			printf("Error type ?? at Line %d: illegal element to calculator.\n", line);
			return NULL;
		}
		return exp1;
	}
	else {
		if(typeEqual(exp1->type, exp2->type) == 0){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n", line);
			return NULL;
		}
		return generateField("relopResult", typeList[0]);
	}
}

FieldList checkExpID(char* exp, int line){
	//printf("CheckExpID:%s\n",exp);
	if(!exp)
		return NULL;
	FieldList f= getSymbol(exp);
	if(!f){
		printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", line, exp);
		return NULL;
	}
	return f;

}

FieldList checkExpArray(FieldList array, FieldList index, int line){
	//printf("CheckExpArray:%s,%s\n",exp1,exp2);
	if(!array || !index)
		return NULL;
	if(array->type->kind != ARRAY){
		printf("Error type 10 at Line %d: \"%s\" is not an array.\n", line, array->name);
		return NULL;
	}
	if(index->type->kind != BASIC || index->type->u.basic == 1){
		printf("Error type 12 at Line %d: \"%s\" is not an integer.\n", line, index->name);
		return NULL;
	}
	return generateField("subarray", array->type->u.array.elem);
}

FieldList checkExpStruct(FieldList exp1, char* name, int line){
	//printf("CheckExpStruct:%s,%s\n",exp1,exp2);
	if(!exp1 || !name)
		return NULL;
	if(exp1->type->kind != STRUCTURE){
		printf("Error type 13 at Line %d: Illegal use of \".\".\n", line);
		return NULL;
	}
	FieldList t = exp1->type->u.structure;
	while(t){
		//printf("%s %s\n",t->name,exp2);
		if(!strcmp(t->name, name))
			return t;
		t = t->next;
	}
	printf("Error type 14 at Line %d: Non-existent field \"%s\".\n", line, name);
	return NULL;
}

FieldList checkExpFunc(char* funcName, FieldList parameter, int line){
	//printf("CheckExpFunc:%s\n",exp);
	if(!funcName)
		return NULL;
	FuncList f = getFuncAddress(funcName);
	if(!f){
		if(getSymbol(funcName) || getTypeAddress(funcName, line, 0))
			printf("Error type 11 at Line %d: \"%s\" is not a function\n", line, funcName);
		else
			printf("Error type 2 at Line %d: Undefined function \"%s\".\n", line, funcName);
		return NULL;
	}
	else {
		if(checkParameter(funcName, f->parameters, parameter, line, 1) == 1) {
			FieldList return_f = generateField("return", f->return_type);
			return return_f;
		}
		else	return NULL;
	}
}

void checkIfType(FieldList f) {
	if(!f || f->type->kind != BASIC || f->type->u.basic != 0) {
		//TODO: if(a) a isn't int
	}
}

void checkReturnType(Type request, FieldList f) {
	while(f) {
		if(typeEqual(request, f->type) == 0) {
			//TODO: return type error
			printf("Error type 8 at Line %d: Type mismatched for return\n", atoi(f->name));
		}
		f = f->next;
	}
}

void Split0(char* string) {
	for(int i = 0;string[i] != '\0';)
		if(string[i] == '0') string[i] = '\0';
		else i++;
}
