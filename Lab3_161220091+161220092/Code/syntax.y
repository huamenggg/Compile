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
			insertSymbol(f, @1.first_line, 1);
			f = f->next;
		}
	}
        | Specifier SEMI
	{
	}
	| Specifier FunDec CompSt
	{
		$2->return_type = $1;
		$2->status = DEF;
		insertFunc($2, @1.first_line);
		checkReturnType($1, $3);
	}
	/*| Specifier FunDec SEMI
	{
		$2->return_type = $1;
		$2->status = DEC;
		insertFunc($2, @1.first_line);
		changeFieldToDec($2->parameters);
		errorLength = 0;
	}
	| Comment
	| error SEMI
	{
		errorNum++;
		printf(RED"Error type B"NONE" at line"RED" %d"NONE
				": something wrong before ';'\n", @2.first_line);
	}*/
	;
ExtDecList : VarDec 
	   {
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
	  }
	  ;
StructSpecifier : STRUCT OptTag LC DefList RC
		{
			/* struct defination */
			$$ = generateType($2, $4);	
			insertType($$, @1.first_line);
			printStructError();
			printStructFollowEqualError($4);
		}
		| STRUCT Tag
		{
			/* struct usage */
			$$ = getTypeAddress($2, @1.first_line, 1);
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
		char name[20];
		sprintf(name, "Unname%d", @$.first_line);
		$$ = name; 
	}
	;
Tag	: ID
	{
		$$ = Filter($1);
	}
    	;

 /* Declarators */
VarDec	: ID 
        {
		$$ = generateField(Filter($1), NULL);	
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
		f->status = DEFVAR;
		if(insertSymbol(f, @1.first_line, 0) == NULL) {
			f->line = @1.first_line;
			errorSymbol[errorLength] = f;
			errorLength++;
		}
		$$ = $2;
	}
	;

 /* Statements */
CompSt	: LC DefList StmtList RC
	{
		printVarError();
		$$ = $3;
	}
        ;
StmtList: Stmt StmtList
	{
		if($1 == NULL) $1 = $2;
		else {
			FieldList f = $1;
			while(f->next != NULL) f = f->next;
			f->next = $2;
		}
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
		if($2 != NULL) {
			$$ = generateField("return", $2->type);
			$$->line = @1.first_line;
		}
	}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	{
		checkIfType($3, @1.first_line);
		$$ = $5;
	}
	| IF LP Exp RP Stmt ELSE Stmt
	{
		checkIfType($3, @1.first_line);
		if($5 == NULL) $5 = $7;
		else $5->next = $7;
		$$ = $5;
	}
	| WHILE LP Exp RP Stmt
	{
		checkIfType($3, @1.first_line);
		$$ = $5;
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
			f->status = DEFVAR;
			if(insertSymbol(f, @1.first_line, 0) == NULL) {
				f->line = @1.first_line;
				errorSymbol[errorLength] = f;
				errorLength++;
			}
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
		strcpy($1->isFollowEqual, "true");
		$1->line = @1.first_line;
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
		$$ = $2;			
	}
	| NOT Exp
	{
		$$ = $2;
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
		$$ = copyField($1);
	}
	;
%%

