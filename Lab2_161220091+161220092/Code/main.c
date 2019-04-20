#include "stdio.h"
#include "syntax.tab.h"
#include "semantic.h"
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
			if(f->type->kind != BASIC)
				printf("\ttype:%s", f->type->name);
			printf("\n");
			f = f->next;
			j++;
			}
		}
	}
}

int main(int argc, char** argv) {
	if(argc <= 1) return 1;
	FILE *f = fopen(argv[1], "r");
       	if(!f) {
		perror(argv[1]);
		return 1;
	}
	initSemantic();
	yyrestart(f);
	yyparse();
	testResult();
	return 0;
}
