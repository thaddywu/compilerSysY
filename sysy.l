%{
#include "sysy.tab.hpp"
#include "defs.hpp"
using namespace std;

int _sysy_val;
string _sysy_str;
%}
%option yylineno
%x COMMENT

digit [0-9]
letter [a-zA-Z_]
delim [ \r\t\n]
ws {delim}+
hex 0[Xx][0-9A-Fa-f]+
oct 0[0-7]*
dec [1-9][0-9]*

%%

"//".* {}
"/*" { BEGIN COMMENT; }
<COMMENT>"*/" { BEGIN INITIAL; }
<COMMENT>. {}
<COMMENT>"\n" {}

{ws} {}

"if" { return IF; }
"else" { return ELSE; }
"while" { return WHILE; }
"break" { return BREAK; }
"return" { return RETURN; }
"continue" { return CONTINUE; }

"==" { return EQ; }
"!=" { return NEQ; }
"<" { return LT; }
">" { return GT; }
"<=" { return LE; }
">=" { return GE; }

"+" { return ADD; }
"-" { return SUB; }
"*" { return MUL; }
"/" { return DIV; }
"%" { return MOD; }

"!" { return NOT; }
"&&" { return AND; }
"||" { return OR; }

"=" { return ASSIGN; }
"const" { return CONST; }
"int" { return INT; }
"void" { return VOID; }

"," { return yytext[0]; }
";" { return yytext[0]; }
"(" { return yytext[0]; }
")" { return yytext[0]; }
"[" { return yytext[0]; }
"]" { return yytext[0]; }
"{" { return yytext[0]; }
"}" { return yytext[0]; }

{hex} { _sysy_val = strtol(yytext, NULL, 16); return INTEGER; }
{oct} { _sysy_val = strtol(yytext, NULL, 8); return INTEGER; }
{dec} { _sysy_val = strtol(yytext, NULL, 10); return INTEGER; }
{letter}({digit}|{letter})* { _sysy_str = yytext; return KEY; }

%%

int yywrap()
{
    return 1;
}
void yyerror(char *message)
{
    printf("Error occurs in Line %d: %s\n", yylineno, message);
}