//#include "lex.yy.c"
%define api.value.type {struct YYSTYPE}
%locations
%{
 #include "lex.yy.c"
 #include "semantic.h"
 #include <stdio.h>
 #include "string.h"
 void yyerror(const char* s);
 int errorNum = 0;
 #define RED "\033[0;32;31m"
 #define NONE "\033[m"
%}

 /* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token <type_char> PLUS MINUS STAR DIV
%token <type_char> AND OR
%token <type_char> DOT NOT LP RP LB RB LC RC
%token <type_char> STRUCT RETURN IF ELSE WHILE BLANK
%token <type_char> SEMI COMMA RELOP ASSIGNOP
%token <type_char> ID
%token <type_type> TYPE

/* declared non-terminals */
%type <type_type> Specifier StructSpecifier
%type <type_field> DefList Def DecList Dec VarDec
%type <type_field> ExtDef ExtDefList ExtDecList Program
%type <type_field> VarList ParamDec
%type <type_field> CompSt StmtList Stmt
%type <type_field> Exp Args /*Comment*/
%type <type_char> OptTag Tag
%type <type_func> FunDec

/* priority and combination*/
%nonassoc error
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left START DIV
%right NOT
%left DOT LP RP LB RB
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
 /* High-level Definitions */
Program	: ExtDefList 
	{
	}
	;
ExtDefList : ExtDef ExtDefList 
	   {
	   }
	   | { $$ = NULL; }
	   ;
ExtDef	: Specifier ExtDecList SEMI 
        {
		FieldList f = $2;
		while(f != NULL) {
			if(f->type == NULL) {
				f->type = $1;
			}
			else {
				Type t = f->type;
				while(t->u.array.elem != NULL) {
					t = t->u.array.elem;
				}
				t->u.array.elem = $1;
			}
			insertSymbol(f, @1.first_line);
			f = f->next;
		}
	}
        | Specifier SEMI
	{
		//TODO: finish
	}
	| Specifier FunDec CompSt
	{
		$2->return_type = $1;
		$2->status = DEF;
		insertFunc($2, @1.first_line);
		checkReturnType($1, $3);
	}
	| Specifier FunDec SEMI
	{
		$2->return_type = $1;
		$2->status = DEC;
		insertFunc($2, @1.first_line);
	}
	/*| Comment
	| error SEMI
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before ';'\n", @2.first_line);
	}*/
	;
ExtDecList : VarDec 
	   {
		//printf("I'm in ExtDecList\n");
		$$ = $1;
	   }
	   | VarDec COMMA ExtDecList
	   {
		$1->next = $3;
		$$ = $1;	
	   }
	   ;

 /* Specifiers */
Specifier : TYPE
	  {
		$$ = $1;
	  }
	  | StructSpecifier
	  {
		$$ = $1;
		//printf("typeLength:%d\n", typeLength);
	  }
	  ;
StructSpecifier : STRUCT OptTag LC DefList RC
		{
			/* struct defination */
			$$ = generateType($2, $4);
			insertType($$, @1.first_line);
			/*FieldList temp = $4;
			while(temp != NULL) {
				printf("name:%s\n", temp->name);
				if(temp->type->kind == STRUCTURE)
					printf("typeName:%s\n", temp->type->u.structure->name);
				temp = temp->next;
			}*/
		}
		| STRUCT Tag
		{
			/* struct usage */
			$$ = getTypeAddress($2, @1.first_line, 1);
			//printf("name:%s\n", $$->name);
		}
		/*| error RC
		{
			errorNum++;
			printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before '}'\n", @2.first_line);
		}*/
		;
OptTag	: ID
	{
		$$ = Filter($1);
	}
        | 
	{ 
		/* TODO:unname struct need to be considered */
		$$ = NULL; 
	}
	;
Tag	: ID
	{
		$$ = Filter($1);
		//printf("ID:%s\n", $$);
	}
    	;

 /* Declarators */
VarDec	: ID 
        {
		//printf("i'm in VarDec\n");
		$$ = generateField(Filter($1), NULL);
		//printf("$$->name:%s\n", $$->name);	
	}
        | VarDec LB INT RB
	{
		Type t = generateTypeArray($3);
		Type t2 = $1->type;
		if(t2 == NULL)
			$1->type = t;
		else {
			while(t2->u.array.elem != NULL)
				t2 = t2->u.array.elem;
			t2->u.array.elem = t;
		}
		$$ = $1;
	}
	/*| error RB
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before ']'\n", @2.first_line);
	}*/
	;
FunDec	: ID LP VarList RP
	{
		$$ = generateFunc(Filter($1), $3, NULL);
	}
        | ID LP RP
	{
		$$ = generateFunc(Filter($1), NULL, NULL);
	}
	/*| error RP
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before ')'\n", @2.first_line);
	}*/
	;
VarList : ParamDec COMMA VarList
	{
		$1->next = $3;
		$$ = $1;
	}
	| ParamDec
	{
		$$ = $1;
	}
	;
ParamDec: Specifier VarDec
	{
		FieldList f = $2;
		if(f->type == NULL) {
			f->type = $1;
		}
		else {
			Type t = f->type;
			while(t->u.array.elem != NULL)
				t = t->u.array.elem;
			t->u.array.elem = $1;
		}
		insertSymbol(f, @1.first_line);
		$$ = $2;
	}
	;

 /* Statements */
CompSt	: LC DefList StmtList RC
	{
		$$ = $3;
	}
	/*| error DefList StmtList RC
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before '}'\n", @4.first_line);
	}*/
        ;
StmtList: Stmt StmtList
	{
		if($1 == NULL) $1 = $2;
		else $1->next = $2;
		$$ = $1;
	}
	| { $$ = NULL; }
	;
Stmt	: Exp SEMI
	{
		$$ = NULL;
	}
     	| CompSt
	{
		$$ = $1;
	}
	| RETURN Exp SEMI
	{
		char returnLine[50];
		sprintf(returnLine, "%d", @1.first_line);
		strcpy($2->name, returnLine);
		$$ = $2;
	}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	{
		checkIfType($3);
		$$ = NULL;
	}
	| IF LP Exp RP Stmt ELSE Stmt
	{
		checkIfType($3);
		$$ = NULL;
	}
	| WHILE LP Exp RP Stmt
	{
		checkIfType($3);
		$$ = NULL;
	}
	/*| Comment
	| error SEMI
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": Missing ';'\n", @2.first_line);
	}
	| error RP
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before ')'\n", @2.first_line);
	}*/
	;

 /* Local Definitions */
DefList : Def DefList
	{
		FieldList temp = $1;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = $2;
		$$ = $1;
	}
	| { $$ = NULL; }
	;
Def	: Specifier DecList SEMI
	{
		/* int a, b = 1, c[2]; */
		/* $2 is a FieldList named a, next is b, and b->next is c */
		FieldList f = $2;
		while(f != NULL) {
			if(f->type == NULL) {
				f->type = $1;
			}
			else {
				Type t = f->type;
				while(t->u.array.elem != NULL)
					t = t->u.array.elem;
				t->u.array.elem = $1;
			}
			insertSymbol(f, @1.first_line);
			f = f->next;
		}
		$$ = $2;
	}
	/*| Comment
	| error SEMI
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before ';'\n", @2.first_line);
	}*/
    	;
DecList : Dec
	{
		$$ = $1;
	}
	| Dec COMMA DecList
	{
		$1->next = $3;
		$$ = $1;
	}
	;
Dec	: VarDec
	{
		$$ = $1;
	}
    	| VarDec ASSIGNOP Exp
	{
		//TODO: if in struct need to error
		$$ = $1;
	}
	/*| Comment*/
	;

 /* Expressions */
Exp	: Exp ASSIGNOP Exp
	{
		$$ = checkExp($1, $3, "ASSIGNOP", @1.first_line);		
	}
    	| Exp AND Exp
	{
		$$ = checkExp($1, $3, "AND", @1.first_line);		
	}
	| Exp OR Exp
	{
		$$ = checkExp($1, $3, "OR", @1.first_line);		
	}
    	| Exp RELOP Exp
	{
		$$ = checkExp($1, $3, "RELOP", @1.first_line);		
	}
    	| Exp PLUS Exp
	{
		$$ = checkExp($1, $3, "PLUS", @1.first_line);		
	}
    	| Exp MINUS Exp
	{
		$$ = checkExp($1, $3, "MINUS", @1.first_line);		
	}
    	| Exp STAR Exp
	{
		$$ = checkExp($1, $3, "STAR", @1.first_line);		
	}
    	| Exp DIV Exp
	{
		$$ = checkExp($1, $3, "DIV", @1.first_line);		
	}
    	| LP Exp RP
	{
				
	}
	| MINUS Exp
	{
		//TODO: if need to judge Exp type			
	}
	| NOT Exp
	{
		
	}
	| ID LP Args RP
	{
		$$ = checkExpFunc(Filter($1), $3, @1.first_line);
	}
	| ID LP RP
	{
		$$ = checkExpFunc(Filter($1), NULL, @1.first_line);	
	}
	| Exp LB Exp RB
	{
		$$ = checkExpArray($1, $3, @1.first_line);
		if($$ != NULL)
			strcpy($$->isLeftValue, "true");
	}
	| Exp DOT ID
	{
		$$ = checkExpStruct($1, Filter($3), @1.first_line);
		if($$ != NULL)
			strcpy($$->isLeftValue, "true");
	}
	| ID
	{
		$$ = checkExpID(Filter($1), @1.first_line);
		if($$ != NULL)
			strcpy($$->isLeftValue, "true");
	}
	| INT
	{
		char int2char[50];
		sprintf(int2char, "%d", $1);	
		$$ = generateField(int2char, typeList[0]);		
	}
	| FLOAT
	{
		char float2char[100];
		sprintf(float2char, "%f", $1);
		Split0(float2char);
		$$ = generateField(float2char, typeList[1]);		
	}
	/*| Comment
	| error RB 
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": Missing '['\n", @2.first_line);
	}*/
	;
/*Comment : STAR DIV
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": not allowed single \n", @1.first_line);
	}
	;*/
Args	: Exp COMMA Args
	{
		$1->next = $3;
		$$ = $1;
	}
     	| Exp
	{
		$$ = $1;	
	}
	;
%%

