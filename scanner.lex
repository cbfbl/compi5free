%{

/* Declarations section */
#include "types.hpp"
#include "parser.tab.hpp"
#include "hw3_output.hpp"

%}

%option yylineno
%option noyywrap

%%



void                            {yylval=new Basictype(yytext); return VOID;};
int                             {yylval=new Basictype(yytext); return INT;};
byte                            {yylval=new Basictype(yytext); return BYTE;};
b                               {yylval=new Basictype(yytext); return B;};
bool                            {yylval=new Basictype(yytext); return BOOL;};
and                             {yylval=new Basictype(yytext); return AND;};
or                              {yylval=new Basictype(yytext); return OR;};
not                             {yylval=new Basictype(yytext); return NOT;};
true                            {yylval=new Bool(yytext); return TRUE;};
false                           {yylval=new Bool(yytext); return FALSE;};
return                          {yylval=new Basictype(yytext); return RETURN;};
if                              {yylval=new Basictype(yytext); return IF;};
else                            {yylval=new Basictype(yytext); return ELSE;};
while                           {yylval=new Basictype(yytext); return WHILE;};
break                           {yylval=new Basictype(yytext); return BREAK;};
continue                        {yylval=new Basictype(yytext); return CONTINUE;};
;                               {yylval=new Basictype(yytext); return SC;};
,                               {yylval=new Basictype(yytext); return COMMA;};
\(                              {yylval=new Basictype(yytext); return LPAREN;};
\)                              {yylval=new Basictype(yytext); return RPAREN;};
\{                              {yylval=new Basictype(yytext); return LBRACE;};
\}                              {yylval=new Basictype(yytext); return RBRACE;};
=                               {yylval=new Basictype(yytext); return ASSIGN;};
\<|>|<=|>=                      {yylval=new Basictype(yytext); return RELOP;};
==|!=                           {yylval=new Basictype(yytext); return RELEQ;};
[\*\/]                          {yylval=new Basictype(yytext); return BINOPH;};
[\+\-]                          {yylval=new Basictype(yytext); return BINOPL;};
[a-zA-z][a-zA-z0-9]*            {yylval=new Id(yytext); return ID;};
0|[1-9][0-9]*                   {yylval=new Num(yytext); return NUM;};
\"([^\n\r\"\\]|\\[rnt"\\])+\"	{yylval=new String(yytext); return STRING;};
<<EOF>>                         {yylval=new Basictype(yytext); return 0;};
[\r\n\t ]|\/\/[^\r\n]*[\r|\n|\r\n]?	    ;
.                               output::errorLex(yylineno); exit(0);


%%
