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
	}
	/* TODO:handle error redefination */
	return s;
}

FieldList getSymbol(char* name) {
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

int typeEqual(Type t1, Type t2){
	if((t1==NULL)||(t2==NULL))
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
