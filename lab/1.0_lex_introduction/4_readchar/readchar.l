%{
    // variable declaration declaration
%}

%%

[a-z] { printf("char: %s - asci: %d\n", yytext, (int)yytext[0]); }
.
\n 

%%

