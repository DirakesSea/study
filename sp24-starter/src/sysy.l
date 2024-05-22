%option noinput
%option nounput
%option noyywrap

%{
#include "sysy.tab.hh"
#include "ast_node.hh"

%}

intnum [1-9][0-9]*|0[0-7]*|(0x|0X)[0-9a-fA-F]*
digit [0-9]
nodigit [_a-zA-Z]
identifier {nodigit}({nodigit}|{digit})*
type  "int"
%x COMMENTS
%x LINECOMMENT

%%



"/*"		{BEGIN(COMMENTS);}
<COMMENTS>"*/"  {BEGIN(INITIAL);}

<COMMENTS>.  {}
"//"         { BEGIN(LINECOMMENT); //printf("<linecomment>\n");
 }  
<LINECOMMENT>[^\n]* {}  
<LINECOMMENT>\n { BEGIN(INITIAL); yylineno++; } 



[ \t]			{}		
<INITIAL,COMMENTS,LINECOMMENT>\n			    { //printf("<line:%d>\n",yylineno++); 
                                                yylineno++; } 
{type}          { //printf("int\n"); 
yylval.node = new_node("type",TOKEN_TYPE,yytext); return TYPE; }
"while"         { //printf("while\n");
 yylval.node = new_node("while",TOKEN_OTHER,yytext); return WHILE; }
"if"            { //printf("if\n");
 yylval.node = new_node("if",TOKEN_OTHER,yytext); return IF; }
"else"          { //printf("else\n");
 yylval.node = new_node("else",TOKEN_OTHER,yytext); return ELSE; }
"void"          { //printf("void\n");
 yylval.node = new_node("void",TOKEN_TYPE,yytext); return VOID; }
"return"        { //printf("return\n");
 yylval.node = new_node("ret",TOKEN_OTHER,yytext); return RET; }
{intnum}        { //printf("intnum\n"); 
yylval.node = new_node("intnum",TOKEN_NUM,yytext); return INTNUM; } 
">"             { //printf(">\n");
 yylval.node = new_node("gt",TOKEN_OTHER,yytext); return GT; }
">="            { //printf(">=\n");
 yylval.node = new_node("ge",TOKEN_OTHER,yytext); return GE; }
"<"             { //printf("<\n");
 yylval.node = new_node("lt",TOKEN_OTHER,yytext); return LT; }
"<="            { //printf("<=\n");
 yylval.node = new_node("le",TOKEN_OTHER,yytext); return LE; }
"+"             { //printf("+\n");
 yylval.node = new_node("add",TOKEN_OTHER,yytext); return ADD; }
"-"             { //printf("-\n");
 yylval.node = new_node("sub",TOKEN_OTHER,yytext); return SUB; }
"*"             { //printf("*\n");
 yylval.node = new_node("mul",TOKEN_OTHER,yytext);return MUL; }
"/"             { //printf("/\n");
 yylval.node = new_node("div",TOKEN_OTHER,yytext); return DIV; }
"%"             { //printf("rem\n");
 yylval.node = new_node("rem",TOKEN_OTHER,yytext); return REM; }
"=="            { //printf("==\n"); 
yylval.node = new_node("eq",TOKEN_OTHER,yytext); return EQ; }
"&&"            { //printf("&&\n");
 yylval.node = new_node("and",TOKEN_OTHER,yytext); return AND; }
"||"            { //printf("||\n");
 yylval.node = new_node("or",TOKEN_OTHER,yytext); return OR; }
"="             { //printf("=\n");
 yylval.node = new_node("assign",TOKEN_OTHER,yytext); return ASSIGN; }
"!="            { //printf("!=\n");
 yylval.node = new_node("neq",TOKEN_OTHER,yytext);return NEQ; }    
"!"             { //printf("!\n");
 yylval.node = new_node("not",TOKEN_OTHER,yytext); return NOT; }
"("             { //printf("(\n");
 yylval.node = new_node("lparent",TOKEN_OTHER,yytext); return LPARENT; }
")"             { //printf(")\n");
 yylval.node = new_node("rparent",TOKEN_OTHER,yytext); return RPARENT; }
"["             { //printf("[\n"); 
yylval.node = new_node("lbracket",TOKEN_OTHER,yytext); return LBRACKET; }
"]"             { //printf("]\n");
 yylval.node = new_node("rbracket",TOKEN_OTHER,yytext); return RBRACKET; }
"{"             { //printf("{\n"); 
yylval.node = new_node("lbrace",TOKEN_OTHER,yytext); return LBRACE; }
"}"             { //printf("}\n");
 yylval.node = new_node("rbrace",TOKEN_OTHER,yytext); return RBRACE; }
";"             { //printf(";\n");
 yylval.node = new_node("semicn",TOKEN_OTHER,yytext); return SEMICN; }
","             { //printf(",\n");
 yylval.node = new_node("comma",TOKEN_OTHER,yytext); return COMMA; }
{identifier}    { //printf("ident\n");
 yylval.node = new_node("id",TOKEN_ID,yytext); return ID; }
.               { printf("ERROR(%s)\n", yytext); }

%%
