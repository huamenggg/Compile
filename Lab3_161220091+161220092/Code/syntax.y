//#include "lex.yy.c"
%define api.value.type {struct YYSTYPE}
%locations
%{
 #include "lex.yy.c"
 #include "semantic.h"
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
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		n->node = CreateVariable("Program", 1, &$1->node);
		root = n->node;
	}
	;
ExtDefList : ExtDef ExtDefList 
	   {
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		Node child[2];
		child[0] = $1->node;
		child[1] = $2->node;
		$$->node = CreateVariable("ExtDefList", 2, child);
	   }
	   | 
	   {
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->node = NULL;
	   }
	   ;
ExtDef	: Specifier ExtDecList SEMI 
        {
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		FieldList f = $2->field;
		while(f != NULL) {
			if(f->type == NULL) {
				f->type = $1->type;
			}
			else {
				Type t = f->type;
				while(t->u.array.elem != NULL) {
					t = t->u.array.elem;
				}
				t->u.array.elem = $1->type;
			}
			insertSymbol(f, @1.first_line, 1);
			f = f->next;
		}
		Node child[3];
		child[0] = $1->node;
		child[1] = $2->node;
		child[2] = CreateTerminal("SEMI", Terminal, yytext, 0, 0);
		$$->node = CreateVariable("ExtDef", 3, child);
	}
        | Specifier SEMI
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		Node child[2];
		child[0] = $1->node;
		child[1] = CreateTerminal("SEMI", Terminal, yytext, 0, 0);
		$$->node = CreateVariable("ExtDef", 2, child);
	}
	| Specifier FunDec CompSt
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$2->func->return_type = $1->type;
		$2->func->status = DEF;
		insertFunc($2->func, @1.first_line);
		printFuncError();
		checkReturnType($1->type, $3->field);
		Node child[3];
		child[0] = $1->node;
		child[1] = $2->node;
		child[2] = $3->node;
		$$->node = CreateVariable("ExtDef", 3, child);
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
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $1->field;
		$$->node = CreateVariable("ExtDecList", 1, &$1->node);
	   }
	   | VarDec COMMA ExtDecList
	   {
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$1->field->next = $3->field;
		$$->field = $1->field;
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("COMMA", Terminal, ",", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("ExtDecList", 3, child);	
	   }
	   ;

 /* Specifiers */
Specifier : TYPE
	  {
		SyntaxTypeNode n = (SyntaxTypeNode)malloc(sizeof(struct SyntaxTypeNode_));
		$$ = n;
		$$->type = $1->type;
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("TYPE", TypeN, $1->type->name, 0, 0);
		$$->node = CreateVariable("Specifier", 1, &p);
	  }
	  | StructSpecifier
	  {
		SyntaxTypeNode n = (SyntaxTypeNode)malloc(sizeof(struct SyntaxTypeNode_));
		$$ = n;
		$$->type = $1->type;
		$$->node = CreateVariable("Specifier", 1, &$1->node);
	  }
	  ;
StructSpecifier : STRUCT OptTag LC DefList RC
		{
			SyntaxTypeNode n = (SyntaxTypeNode)malloc(sizeof(struct SyntaxTypeNode_));
			$$ = n;
			/* struct defination */
			$$->type = generateType($2->c, $4->field);	
			insertType($$->type, @1.first_line);
			printStructError();
			printStructFollowEqualError($4->field);
			Node child[5];
			child[0] = CreateTerminal("STRUCT", Terminal,
					"STRUCT", 0, 0);
			child[1] = $2->node;
			child[2] = CreateTerminal("LC", Terminal,
					"{", 0, 0);
			child[3] = $4->node;
			child[4] = CreateTerminal("RC", Terminal,
					"}", 0, 0);
			$$->node = CreateVariable("StructSpecifier", 5, child);
		}
		| STRUCT Tag
		{
			SyntaxTypeNode n = (SyntaxTypeNode)malloc(sizeof(struct SyntaxTypeNode_));
			$$ = n;
			/* struct usage */
			$$->type = getTypeAddress($2->c, @1.first_line, 1);
			Node child[2];
			child[0] = CreateTerminal("STRUCT", Terminal,
					"STRUCT", 0, 0);
			child[1] = $2->node;
			$$->node = CreateVariable("StructSpecifier", 2, child);
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
		SyntaxCharNode n = (SyntaxCharNode)malloc(sizeof(struct SyntaxCharNode_));
		$$ = n;
		$$->c = Filter($1->c);
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("ID", Id, Filter($1->c), 0, 0);
		$$->node = CreateVariable("OptTag", 1, &p);
	}
        | 
	{ 
		SyntaxCharNode n = (SyntaxCharNode)malloc(sizeof(struct SyntaxCharNode_));
		$$ = n;
		char name[20];
		sprintf(name, "Unname%d", @$.first_line);
		$$->c = name; 
		$$->node = NULL;
	}
	;
Tag	: ID
	{
		SyntaxCharNode n = (SyntaxCharNode)malloc(sizeof(struct SyntaxCharNode_));
		$$ = n;
		$$->c = Filter($1->c);
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("ID", Id, Filter($1->c), 0, 0);
		$$->node = CreateVariable("Tag", 1, &p);
	}
    	;

 /* Declarators */
VarDec	: ID 
        {
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		n->field = generateField(Filter($1->c), NULL);
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("ID", Id, Filter($1->c), 0, 0);
		n->node = CreateVariable("VarDec", 1, &p);
		$$ = n;
	}
        | VarDec LB INT RB
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		Type t = generateTypeArray($3);
		Type t2 = $1->field->type;
		if(t2 == NULL)
			$1->field->type = t;
		else {
			while(t2->u.array.elem != NULL)
				t2 = t2->u.array.elem;
			t2->u.array.elem = t;
		}
		$$->field = $1->field;
		Node child[4];
		child[0] = $1->node;
		child[1] = CreateTerminal("LB", Terminal,
				"[", 0, 0);
		child[2] = CreateTerminal("INT", Number,
				"INT", $3, 0);
		child[3] = CreateTerminal("RB", Terminal,
				"]", 0, 0);
		$$->node = CreateVariable("VarDec", 4, child);
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
		SyntaxFuncNode n = (SyntaxFuncNode)malloc(sizeof(struct SyntaxFuncNode_));
		$$ = n;
		$$->func = generateFunc(Filter($1->c), $3->field, NULL);
		Node child[4];
		child[0] = CreateTerminal("ID", Id,
				Filter($1->c), 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = $3->node;
		child[3] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		$$->node = CreateVariable("FunDec", 4, child);
	}
        | ID LP RP
	{
		SyntaxFuncNode n = (SyntaxFuncNode)malloc(sizeof(struct SyntaxFuncNode_));
		$$ = n;
		$$->func = generateFunc(Filter($1->c), NULL, NULL);
		Node child[3];
		child[0] = CreateTerminal("ID", Id,
				Filter($1->c), 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		$$->node = CreateVariable("FunDec", 3, child);
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
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$1->field->next = $3->field;
		$$->field = $1->field;
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("COMMA", Terminal,
				",", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("VarList", 3, child);
	}
	| ParamDec
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $1->field;
		$$->node = CreateVariable("VarList", 1, &$1->node);
	}
	;

ParamDec: Specifier VarDec
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		FieldList f = $2->field;
		if(f->type == NULL) {
			f->type = $1->type;
		}
		else {
			Type t = f->type;
			while(t->u.array.elem != NULL)
				t = t->u.array.elem;
			t->u.array.elem = $1->type;
		}
		f->status = DEFVAR;
		if(insertSymbol(f, @1.first_line, 0) == NULL) {
			f->line = @1.first_line;
			errorSymbol[errorLength] = f;
			errorLength++;
		}
		$$->field = $2->field;
		Node child[2];
		child[0] = $1->node;
		child[1] = $2->node;
		$$->node = CreateVariable("ParamDec", 2, child);
	}
	;

 /* Statements */
CompSt	: LC DefList StmtList RC
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		printVarError();
		$$->field = $3->field;
		Node child[4];
		child[0] = CreateTerminal("LC", Terminal,
				"{", 0, 0);
		child[1] = $2->node;
		child[2] = $3->node;
		child[3] = CreateTerminal("RC", Terminal,
				"}", 0, 0);
		$$->node = CreateVariable("CompSt", 4, child);
	}
        ;
StmtList: Stmt StmtList
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		if($1->field == NULL) $1->field = $2->field;
		else {
			FieldList f = $1->field;
			while(f->next != NULL) f = f->next;
			f->next = $2->field;
		}
		$$->field = $1->field;
		Node child[2];
		child[0] = $1->node;
		child[1] = $2->node;
		$$->node = CreateVariable("StmtList", 2, child);	
	}
	| 
	{ 
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = NULL; $$->node = NULL; 
	}
	;
Stmt	: Exp SEMI
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = NULL;
		Node child[2];
		child[0] = $1->node;
		child[1] = CreateTerminal("SEMI", Terminal, ";", 0, 0);
		$$->node = CreateVariable("Stmt", 2, child);
	}
     	| CompSt
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $1->field;
		$$->node = CreateVariable("Stmt", 1, &$1->node);
	}
	| RETURN Exp SEMI
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		if($2->field != NULL) {
			$$->field = generateField("return", $2->field->type);
			$$->field->line = @1.first_line;
		}
		Node child[3];
		child[0] = CreateTerminal("RETURN", Terminal,
				"RETURN", 0, 0);
		child[1] = $2->node;
		child[2] = CreateTerminal("SEMI", Terminal,
				";", 0, 0);
		$$->node = CreateVariable("Stmt", 3, child);
	}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		checkIfType($3->field, @1.first_line);
		$$->field = $5->field;
		Node child[5];
		child[0] = CreateTerminal("IF", Terminal,
				"IF", 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = $3->node;
		child[3] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		child[4] = $5->node;
		$$->node = CreateVariable("Stmt", 5, child);
	}
	| IF LP Exp RP Stmt ELSE Stmt
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		checkIfType($3->field, @1.first_line);
		if($5->field == NULL) $5->field = $7->field;
		else $5->field->next = $7->field;
		$$->field = $5->field;
		Node child[7];
		child[0] = CreateTerminal("IF", Terminal,
				"IF", 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = $3->node;
		child[3] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		child[4] = $5->node;
		child[5] = CreateTerminal("ELSE", Terminal,
				"ELSE", 0, 0);
		child[6] = $7->node;
		$$->node = CreateVariable("Stmt", 7, child);
	}
	| WHILE LP Exp RP Stmt
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		checkIfType($3->field, @1.first_line);
		$$->field = $5->field;
		Node child[5];
		child[0] = CreateTerminal("WHILE", Terminal,
				"WHILE", 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = $3->node;
		child[3] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		child[4] = $5->node;
		$$->node = CreateVariable("Stmt", 5, child);
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
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		FieldList temp = $1->field;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = $2->field;
		$$->field = $1->field;
		Node child[2];
		child[0] = $1->node;
		child[1] = $2->node;
		$$->node = CreateVariable("DefList", 2, child);
	}
	| 
	{ 
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = NULL; $$->node = NULL; 
	}
	;
Def	: Specifier DecList SEMI
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		/* int a, b = 1, c[2]; */
		/* $2 is a FieldList named a, next is b, and b->next is c */
		FieldList f = $2->field;
		while(f != NULL) {
			if(f->type == NULL) {
				f->type = $1->type;
			}
			else {
				Type t = f->type;
				while(t->u.array.elem != NULL)
					t = t->u.array.elem;
				t->u.array.elem = $1->type;
			}
			f->status = DEFVAR;
			if(insertSymbol(f, @1.first_line, 0) == NULL) {
				f->line = @1.first_line;
				errorSymbol[errorLength] = f;
				errorLength++;
			}
			f = f->next;
		}
		$$->field = $2->field;
		Node child[3];
		child[0] = $1->node;
		child[1] = $2->node;
		child[2] = CreateTerminal("SEMI", Terminal, ";", 0, 0);
		$$->node = CreateVariable("Def", 3, child);
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
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $1->field;
		$$->node = CreateVariable("DecList", 1, &$1->node);
	}
	| Dec COMMA DecList
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$1->field->next = $3->field;
		$$->field = $1->field;
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("COMMA", Terminal,
				",", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("DecList", 3, child);
	}
	;
Dec	: VarDec
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $1->field;
		$$->node = CreateVariable("Dec", 1, &$1->node);
	}
    	| VarDec ASSIGNOP Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		strcpy($1->field->isFollowEqual, "true");
		$1->field->line = @1.first_line;
		$$->field = $1->field;
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("ASSIGNOP", Terminal, "=", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Dec", 3, child);
	}
	/*| Comment*/
	;

 /* Expressions */
Exp	: Exp ASSIGNOP Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "ASSIGNOP", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("ASSIGNOP", Terminal,
				"=", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);
	}
    	| Exp AND Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "AND", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("AND", Terminal,
				"&&", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);		
	}
	| Exp OR Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "OR", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("OR", Terminal,
				"||", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);
	}
    	| Exp RELOP Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "RELOP", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("RELOP", Terminal,
				$2->c, 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);		
	}
    	| Exp PLUS Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "PLUS", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("PLUS", Terminal,
				"+", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);
	}
    	| Exp MINUS Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "MINUS", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("MINUS", Terminal,
				"-", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);	
	}
    	| Exp STAR Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "STAR", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("STAR", Terminal,
				"*", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);	
	}
    	| Exp DIV Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExp($1->field, $3->field, "DIV", @1.first_line);
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("DIV", Terminal,
				"/", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Exp", 3, child);
	}
    	| LP Exp RP
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		Node child[3];
		child[0] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[1] = $2->node;
		child[2] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		$$->node = CreateVariable("Exp", 3, child);
	}
	| MINUS Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $2->field;
		Node child[2];
		child[0] = CreateTerminal("MINUS", Terminal, "-", 0, 0);
		child[1] = $2->node;
		$$->node = CreateVariable("Exp", 2, child);
	}
	| NOT Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = $2->field;
		Node child[2];
		child[0] = CreateTerminal("NOT", Terminal, "~", 0, 0);
		child[1] = $2->node;
		$$->node = CreateVariable("Exp", 2, child);
	}
	| ID LP Args RP
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExpFunc(Filter($1->c), $3->field, @1.first_line);
		Node child[4];
		child[0] = CreateTerminal("ID", Id,
				Filter($1->c), 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = $3->node;
		child[3] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		$$->node = CreateVariable("Exp", 4, child);
	}
	| ID LP RP
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExpFunc(Filter($1->c), NULL, @1.first_line);
		Node child[3];
		child[0] = CreateTerminal("ID", Id,
				Filter($1->c), 0, 0);
		child[1] = CreateTerminal("LP", Terminal,
				"(", 0, 0);
		child[2] = CreateTerminal("RP", Terminal,
				")", 0, 0);
		$$->node = CreateVariable("Exp", 3, child);
	}
	| Exp LB Exp RB
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExpArray($1->field, $3->field, @1.first_line);
		if($$->field != NULL)
			strcpy($$->field->isLeftValue, "true");
		Node child[4];
		child[0] = $1->node;
		child[1] = CreateTerminal("LB", Terminal,
				"[", 0, 0);
		child[2] = $3->node;
		child[3] = CreateTerminal("RB", Terminal,
				"]", 0, 0);
		$$->node = CreateVariable("Exp", 4, child);
	}
	| Exp DOT ID
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExpStruct($1->field, Filter($3->c), @1.first_line);
		if($$->field != NULL)
			strcpy($$->field->isLeftValue, "true");
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("DOT", Terminal,
				".", 0, 0);
		child[2] = CreateTerminal("ID", Id,
				Filter($3->c), 0, 0);
		$$->node = CreateVariable("Exp", 3, child);
	}
	| ID
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = checkExpID(Filter($1->c), @1.first_line);
		if($$->field != NULL)
			strcpy($$->field->isLeftValue, "true");
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("ID", Id, Filter($1->c), 0, 0);
		$$->node = CreateVariable("Exp", 1, &p);
	}
	| INT
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		char int2char[50];
		sprintf(int2char, "%d", $1);	
		$$->field = generateField(int2char, typeList[0]);
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("INT", Number, "INT", $1, 0);
		$$->node = CreateVariable("Exp", 1, &p);		
	}
	| FLOAT
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		char float2char[100];
		sprintf(float2char, "%f", $1);
		Split0(float2char);
		$$->field = generateField(float2char, typeList[1]);
		Node p = (Node)malloc(sizeof(struct Node_));
		p = CreateTerminal("FLOAT", Number, "FLOAT", 0, $1);
		$$->node = CreateVariable("Exp", 1, &p);
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
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$1->field->next = $3->field;
		$$->field = $1->field;
		Node child[3];
		child[0] = $1->node;
		child[1] = CreateTerminal("COMMA", Terminal,
				",", 0, 0);
		child[2] = $3->node;
		$$->node = CreateVariable("Args", 3, child);
	}
     	| Exp
	{
		SyntaxFieldNode n = (SyntaxFieldNode)malloc(sizeof(struct SyntaxFieldNode_));
		$$ = n;
		$$->field = copyField($1->field);
		$$->node = CreateVariable("Args", 1, &$1->node);
	}
	;
%%

