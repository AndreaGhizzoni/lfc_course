%{
    #include "header.h"
    #include "y.tab.h"

    #undef yywrap
    void yyerror(const char *);
%}

%%

">="    return GE;
"<="    return LE;
"=="    return EQ;
"!="    return NE;
"<"     return LT;
">"     return GT;
"while" return WHILE;
"if"    return IF;
"else"  return ELSE;
"print" return PRINT;
"for"   return FOR;
"to"    return TO;
eval    return EVAL;

"TRUE" {
    yylval.bValue=true;
    return BOOLEAN_V;
}

"FALSE" {
    yylval.bValue=false;
    return BOOLEAN_V;
}

CONST return CONST;
int   return INT_T;
float return FLOAT_T;
bool  return BOOL_T;
main  return MAIN;

[a-zA-Z]([a-zA-Z_]*[0-9]*)* {
    symrec* s = (symrec*)malloc (sizeof(symrec));
    s->name = malloc(sizeof(char*)*strlen(yytext));//malloc(strlen(variableName)+1);
    strcpy(s->name,yytext);
    *((symrec**)&yylval) = s;
    return VARIABLE;
}

0 {
    //bool or int?
    yylval.iValue = atoi(yytext);
    return INTEGER_V;
}

[1-9][0-9]* {
    yylval.iValue = atoi(yytext);
    return INTEGER_V;
}

([1-9][0-9]*|0)\.[0-9]+ { 
    yylval.fValue = atof(yytext);
    return FLOAT_V;
}

\+ return PLUS;
\- return MINUS;
\* return MULTIPLY;
\/ return DIVIDE;
=  return EQUALS;

[ \t\n] ;/* skip whitespace */
\; return SEMICOLON;
\, return COMMA;
\( return LP;
\) return RP;
\{ return LCURLY;
\} return RCURLY;
\[ return LBRACK;
\] return RBRACK;
. { 
    ECHO;
    yyerror("Previous char is unknown to lexer");
}

%%
