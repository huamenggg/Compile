#include "syntax.tab.h"
#include "semantic.h"
#include "intermediate.h"
extern int yyrestart(FILE* f);
extern void printSytaxTree();
extern int yylineno;
extern int errorNum;

void yyerror(char* s) {
	fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, s);
	return;
}

void testResult() {
	printf("---------------Type List---------------\n");
	for(int i = 2;i < typeLength;i++) {
		printf("typeName:%s\n", typeList[i]->name);
		FieldList f = typeList[i]->u.structure;
		while(f != NULL){
			printf("\tf->name:%s", f->name);
			if(f->type->kind != BASIC)
				printf("\ttype:%s", f->type->name);
			printf("\n");
			f = f->next;
		}
	}
	printf("--------------Symbol List -------------\n");
	for(int i = 0;i < hashLength;i++){
		if(symbolList[i] != NULL) {
			FieldList f = symbolList[i];
			int j = 1;
			while(f != NULL){
				printf("[%d:%d]name:%s", i, j, f->name);
				if(f->type->kind == STRUCTURE)
					printf("\ttype:%s", f->type->name);
				else if(f->type->kind == ARRAY) {
					Type t1 = f->type;
					while(t1->kind == ARRAY) {
						printf("\t%d", t1->u.array.size);
						t1 = t1->u.array.elem;
					}
					printf("\ttype:%d", t1->kind);
				}
				printf("\n");
				f = f->next;
				j++;
			}
		}
	}
	printf("--------------Function List -------------\n");
	for(int i = 0;i < hashLength;i++){
		if(funcList[i] != NULL) {
			FuncList f = funcList[i];
			int j = 1;
			while(f != NULL){
				printf("[%d:%d]funcName:%s\t", i, j, f->name);
				if(f->return_type == NULL) {
					printf("\n");
					f = f->next;
					continue;
				}
				if(f->return_type->kind == BASIC)
					printf("return type:%d 0 is int 1 is float\n", f->return_type->u.basic);
				else {
					printf("return type:%s\n", f->return_type->name);
				}
				printf("parameters are:");
				FieldList t = f->parameters;
				while(t) {
					if(t->type->kind == BASIC)
						printf("%d\n", t->type->u.basic);
					else if(t->type->kind == ARRAY) {
						Type t1 = t->type;
						while(t1->kind == ARRAY) {
							printf("\t%d", t1->u.array.size);
							t1 = t1->u.array.elem;
						}
						printf("\ttype:%d", t1->kind);
					}
					else
						printf("%s\n", t->type->name);
					printf(" %s,", t->name);
					t = t->next;
				}
				printf("\n");
				f = f->next;
				j++;
			}
		}
	}
}

/*void funcCheck() {
	for(int i = 0;i < hashLength;i++){
		if(funcList[i] != NULL) {
			FuncList f = funcList[i];
			while(f != NULL){
				if(f->status == DEC)
					printf("Error Type "RED"18"NONE" at line "RED"%d"NONE": Undefined function \"%s\".\n", f->line, f->name);
				f = f->next;
			}
		}
	}
}*/

int main(int argc, char** argv) {
	if(argc <= 2) {
		printf("args number error\n");
		return 1;
	}
	FILE *f = fopen(argv[1], "r");
	FILE *f2 = fopen(argv[2], "w");
       	if(!f || !f2) {
		perror(argv[1]);
		return 1;
	}
	initSemantic();
	yyrestart(f);
	yyparse();
	//printSytaxTree();
	InitialInterCodes();
	generateInterCode();
	writeToFile(f2);
//	testResult();
	return 0;
}
