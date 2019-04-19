%{
 #include "syntax.tab.h"
 int yycolumn = 1;
 #define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
 extern char IdString[300];
 extern int errorNum;
 #define RED "\033[0;32;31m"
 #define NONE "\033[m"
%}
digit [0-9]
letter [a-zA-Z]
relop ">"|"<"|">="|"<="|"=="|"!="
blank [" ""\t"]+
int {digit}+
float {int}\.{digit}+
id ("_"|{letter})+({letter}|{digit}|"_")*
%option yylineno
%%
{int} {
	yylval.type_int = atoi(yytext);
	return INT;
}
{float} { 
	yylval.type_float = atof(yytext);
	return FLOAT; 
}
(int|float)/{blank} {
	yylval.type_char = yytext;
	return TYPE;
}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"." {return DOT;}
"!" {return NOT;}
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"{" {return LC;}
"}" {return RC;}
struct/{blank} {return STRUCT;}
return/{blank} {return RETURN;}
if/({blank}|"(") {return IF;}
else/({blank}|"{") {return ELSE;}
while/({blank}|"(") {return WHILE;}
";" {return SEMI;}
"," {return COMMA;}
"=" {return ASSIGNOP;}
{relop} {return RELOP;}
{id} {
	//printf("lexi:%s\n", yytext);
	yylval.type_char = yytext;
	return ID;
}
"//" {
	char c = input();
	while (c != '\n') c = input();
}
"/*" {
	char a = input();//"/"
	char b = input();//"*'
	//printf("a:%c b:%c\n", a, b);
	while(!(a == '*' && b == '/')) {
		a = b;
		b = input();
		//printf("a:%c b:%c\n", a,b);
	};	
}
{blank} {}
\n { yycolumn = 1; }
. {
  printf(RED"Error type A"NONE" at line "RED"%d"NONE": Mysterious characters \'%s\'\n", yylineno, yytext);
  errorNum++;
  }
%%
