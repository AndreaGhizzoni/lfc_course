%{
    // variable declaration declaration
%}

%%

[A-Z] { printf("%c - %c", yytext[0], (yytext[0]-'A'+3)%('Z'-'A'+1)+'A' ); }
[a-z] { printf("%c - %c", yytext[0], (yytext[0]-'a'+3)%('z'-'a'+1)+'a' ); }
[ \n] { ECHO; }
.

%%


