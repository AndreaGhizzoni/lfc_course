%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(char *);
%}

/* Token passed from lexer */
%token INTEGER

%left '-' '+'
%left '*' '/'

%%

program: expr '\n'            { printf("%d\n", $1); }
       | 
       ;

expr: INTEGER
    | expr '+' expr           { $$ = $1 + $3; }
    | expr '-' expr           { $$ = $1 - $3; }
    | expr '*' expr           { $$ = $1 * $3; }
    | expr '/' expr           { $$ = $1 / $3; }
    | '(' expr ')'            { $$ = $2; }
    ;

%%

void 
yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int 
main(void) {
    yyparse();
    return 0;
}
