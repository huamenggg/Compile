//#include "lex.yy.c"
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

 /* declared types */
%union {
 int type_int;
 float type_float;
 char* type_char;
}

 /* declared tokens */
%token <type_int>INT
%token <type_float> FLOAT
%token <type_char> PLUS MINUS STAR DIV
%token <type_char> AND OR
%token <type_char> DOT NOT LP RP LB RB LC RC
%token <type_char> STRUCT RETURN IF ELSE WHILE BLANK
%token <type_char> SEMI COMMA RELOP ASSIGNOP
%token <type_char> ID TYPE

/* declared non-terminals */
%type <type_char> OptTag Tag
%type <type_char> Program ExtDefList ExtDef ExtDecList Specifier
%type <type_char> StructSpecifier VarDec FunDec VarList
%type <type_char> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <type_char> Dec Exp Args /*Comment*/

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
	}
        | Specifier SEMI
	{
	}
	| Specifier FunDec CompSt
	{
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
	   }
	   | VarDec COMMA ExtDecList
	   {
	   }
	   ;

 /* Specifiers */
Specifier : TYPE
	  {
	  }
	  | StructSpecifier
	  {
	  }
	  ;
StructSpecifier : STRUCT OptTag LC DefList RC
		{
		}
		| STRUCT Tag
		{
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
        | { $$ = NULL; }
	;
Tag	: ID
	{
		$$ = Filter($1);
	}
    	;

 /* Declarators */
VarDec	: ID 
        {
	}
        | VarDec LB INT RB
	{
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
	}
        | ID LP RP
	{
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
	}
	| ParamDec
	{
	}
	;
ParamDec: Specifier VarDec
	{
	}
	;

 /* Statements */
CompSt	: LC DefList StmtList RC
	{
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
	}
	| { $$ = NULL; }
	;
Stmt	: Exp SEMI
	{
	}
     	| CompSt
	{
	}
	| RETURN Exp SEMI
	{
	}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	{
	}
	| IF LP Exp RP Stmt ELSE Stmt
	{
	}
	| WHILE LP Exp RP Stmt
	{
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
	}
	| { $$ = NULL; }
	;
Def	: Specifier DecList SEMI
	{
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
	}
	| Dec COMMA DecList
	{
	}
	;
Dec	: VarDec
	{
	}
    	| VarDec ASSIGNOP Exp
	{
	}
	/*| Comment*/
	;

 /* Expressions */
Exp	: Exp ASSIGNOP Exp
	{
	}
    	| Exp AND Exp
	{
	}
	| Exp OR Exp
	{
	}
    	| Exp RELOP Exp
	{
	}
    	| Exp PLUS Exp
	{
	}
    	| Exp MINUS Exp
	{
	}
    	| Exp STAR Exp
	{
	}
    	| Exp DIV Exp
	{
	}
    	| LP Exp RP
	{
	}
	| MINUS Exp
	{
	}
	| NOT Exp
	{
	}
	| ID LP Args RP
	{
	}
	| ID LP RP
	{
	}
	| Exp LB Exp RB
	{
	}
	| Exp DOT ID
	{
	}
	| ID
	{
	}
	| INT
	{
	}
	| FLOAT
	{
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
	}
     	| Exp
	{
	}
	;
%%

