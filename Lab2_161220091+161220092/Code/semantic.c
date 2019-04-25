#include"semantic.h"

void initSemantic() {
	/* initial typeList[0] to 'int' */
	Type t = (Type)malloc(sizeof(struct Type_));
	t->kind = BASIC;
	t->u.basic = 0;
	typeList[0] = t;

	/* initial typeList[1] to 'float' */
	t = (Type)malloc(sizeof(struct Type_));
	t->kind = BASIC;
	t->u.basic = 1;
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

Type getTypeAddress(char* typeName) {
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
	/* TODO: handle undefined type */
	return NULL;
}

Type insertType(Type type) {
	if(getTypeAddress(type->u.structure->name) == NULL) {
		typeList[typeLength] = type;
		typeLength++;
		return type;
	}
	/* TODO: redefination error */
	printf(RED"error: redefination\n"NONE);

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

FieldList insertSymbol(FieldList field) {
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
	printf("Error type 3 at Line ?: Redefined variable '%s'.\n", field->name);
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

FuncList insertFunc(FuncList Func) {
	FuncList f = getFuncAddress(Func->name);
	if(f == NULL) {
		int index = getHashIndex(Func->name);
		f = (FuncList)malloc(sizeof(struct FuncList_));
		strcpy(f->name, Func->name);
		f->return_type = Func->return_type;
		f->parameters = Func->parameters;
		f->next = funcList[index];
		funcList[index] = f;
	}
	else if(f->status == DEC) {
		if(checkFuncEqual(f, Func) == 1)
			f->status = Func->status;
		else
			/*TODO:error*/
			printf("Error type ?? at Line ?: Function declaration mismatch with defination.\n");
	}
	else {
		/* TODO:line */
		printf("Error type 4 at Line ?: Redefined function '%s'.\n", Func->name);
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

int checkFuncEqual(FuncList f1, FuncList f2) {
	if(typeEqual(f1->return_type, f2->return_type) == 0) return 0;
	FieldList p1 = f1->parameters, p2 = f2->parameters;
	while(p1 != NULL && p2 != NULL) {
		if(typeEqual(p1->type, p2->type) == 0) return 0;
		p1 = p1->next;
		p2 = p2->next;
	}
	if(p1 == NULL && p2 == NULL) return 1;
	return 0;	
}

int typeEqual(Type t1, Type t2){
	if(!(t1&&t2))
		return 0;
	if(t1->kind==t2->kind){
		if(t1->kind==BASIC){
			if(t1->u.basic==t2->u.basic)
				return 1;
		}
		else if(t1->kind==ARRAY){
			if(typeEqual(t1->u.array.elem,t2->u.array.elem))
				return 1;
		}
		else if(t1->kind==STRUCTURE){
			FieldList f1=t1->u.structure;
			FieldList f2=t2->u.structure;
			if(!f1&&!f2)
				return 1;
			else if(f1&&f2){
				while(f1&&f2){
					if(!typeEqual(f1->type,f2->type))
						break;
					f1=f1->next;
					f2=f2->next;	
				}
				if(!f1&&!f2)
					return 1;
			}
		}
		//else t1->kind==FUNCTION
	}
	return 0;
}

char* checkExp(char* exp1,char* exp2,char* error,int line){
	printf("CheckExp:%s,%s,%s\n",exp1,exp2,error);
	if(!strcmp(error,"ASSIGNOP")||
		!strcmp(error,"AND")||
		!strcmp(error,"OR")||
		!strcmp(error,"RELOP")||
		!strcmp(error,"PLUS")||
		!strcmp(error,"MINUS")||
		!strcmp(error,"STAR")||
		!strcmp(error,"DIV")){
		return checkExp2(exp1,exp2,error,line);
	}
	else if(!strcmp(error,"ARRAY")){
		return checkExpArray(exp1,exp2,line);
	}
	else if(!strcmp(error,"STRUCT")){
		return checkExpStruct(exp1,exp2,line);
	}
	else if(!strcmp(error,"ID")){
		return checkExpID(exp1,line);
	}
	else if(!strcmp(error,"FUNC")){
		return checkExpFunc(exp1,line);
	}
}

char* checkExp2(char* exp1,char* exp2,char* error,int line){
	printf("CheckExp%s:%s,%s\n",error,exp1,exp2);
	if(!exp1||!exp2)
		return NULL;
	if(!strcmp(error,"ASSIGNOP")){
		FieldList f1=getSymbol(exp1);
	     	FieldList f2=getSymbol(exp2);
		if(!f1||!f2){
			//printf("%s,%s\n",exp1,exp2);
			
			if(!strcmp(exp1,"INT")||!strcmp(exp1,"FLOAT")){
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", line);
				return NULL;
			}
			int temp1=0,temp2=0;
			//printf("??\n");
			if(f1)
				temp1=f1->type->u.basic;
			else{
				if(!strcmp(exp1,"FLOAT"))
					temp1=1;
				else if(!strcmp(exp1,"INT"))
					temp1=0;
				//else
				//	return NULL;
			}
			if(f2)
				temp2=f2->type->u.basic;
			else{
			       	if(!strcmp(exp2,"FLOAT"))
					temp2=1;
				else if(!strcmp(exp1,"INT"))
					temp2=0;
				//else 
				//	return NULL;
			}
			if(temp1!=temp2){
				printf("Error type 5 at Line %d: Type mismatched for assignment.\n", line);	
				return NULL;
			}
		}
		else if(typeEqual(f1->type,f2->type)){
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", line);
			return NULL;
		}
	} 
	else if(!strcmp(error,"AND")||
		!strcmp(error,"OR")||
		!strcmp(error,"RELOP")||
		!strcmp(error,"PLUS")||
		!strcmp(error,"MINUS")||
		!strcmp(error,"STAR")||
		!strcmp(error,"DIV")){
		FieldList f1=getSymbol(exp1);
	     	FieldList f2=getSymbol(exp2);
		//Type t1,t2;
		if(!f1||!f2){
			//printf("%s\n",exp1);
			int temp1=-1,temp2=-1;
			if(f1){
				//printf("??\n");
				temp1=f1->type->u.basic;
			}else{
				if(!strcmp(exp1,"FLOAT"))
					temp1=1;
				else if(!strcmp(exp1,"INT"))
					temp1=0;
				//else 
				//	return NULL;
			}
			if(f2)
				temp2=f2->type->u.basic;
			else{
			       	if(!strcmp(exp2,"FLOAT"))
					temp2=1;
				else if(!strcmp(exp2,"INT"))
					temp2=0;
				//else 
				//	return NULL;
			}
			if(temp1!=temp2){
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", line);
				return NULL;
			}
		}
		else if(!typeEqual(f1->type,f2->type)){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n", line);
			return NULL;
		}
	}
	return exp1;

}

char* checkExpID(char* exp, int line){
	printf("CheckExpID:%s\n",exp);
	if(!exp)
		return NULL;
	FieldList f=getSymbol(exp);
	if(!f&&exp!="INT"&&exp!="FLOAT"){
		printf("Error type 1 at Line %d: Undefined variable '%s'.\n", line, exp);
		return NULL;
	}
	return exp;

}

char* checkExpArray(char* exp1,char* exp2,int line){
	printf("CheckExpArray:%s,%s\n",exp1,exp2);
	if(!exp1||!exp2)
		return NULL;
	FieldList f1=getSymbol(exp1);
	FieldList f2=getSymbol(exp2);
	printf("%d\n",f1->type->kind);
	if(f1&&f1->type->kind!=ARRAY){
		printf("Error type 10 at Line %d: '%s' is not an array.\n", line, exp1);
		return NULL;
	}
	if(f2&&f2->type->u.basic==1){//float
		printf("Error type 12 at Line %d: '%f' is not an integer.\n", line, exp2);
		return NULL;
	}
	else if(!strcmp(exp2,"FLOAT")){
		printf("Error type 12 at Line %d: '%f' is not an integer.\n", line, exp2);
		return NULL;	
	}
	return exp1;
}

char* checkExpStruct(char* exp1,char* exp2,int line){
	printf("CheckExpStruct:%s,%s\n",exp1,exp2);
	if(!exp1||!exp2)
		return NULL;
	FieldList f1=getSymbol(exp1);
	//FieldList f2=getSymbol(exp2);
	//printf("%s\n",exp2);
	if(f1&&f1->type->kind!=STRUCTURE){
		printf("Error type 13 at Line %d: Illegal use of '.'.\n", line);
		return NULL;
	}
	else if(f1){
		FieldList t=f1->type->u.structure;
		while(t){
			//printf("%s %s\n",t->name,exp2);
			if(!strcmp(t->name,exp2))
				return t->name;
			t=t->next;
		}
		if(!t){
			printf("Error type 14 at Line %d: Non-existent field '%s'.\n", line, exp2);
			return NULL;
		}
	}
	return exp2;
}

char* checkExpFunc(char* exp, int line){
	printf("CheckExpFunc:%s\n",exp);
	if(!exp)
		return NULL;
	FuncList f=getFuncAddress(exp);
	if(!f){
		printf("Error type 2 at Line %d: Undefined function '%s'.\n", line, exp);
		return NULL;
	}
	else 
		return exp;
}

