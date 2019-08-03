%{
    // variable declaration declaration
%}

%%
[0-9] {printf("%s", yytext);}
","   {printf("</td><td>");}
\n    {printf("</td></tr><tr><td>");}
.

%%

int main(){
    printf("<html><head></head><body>");
    printf("<table><tr><td>");
    yylex();
    printf("</td></tr></table>");
    printf("</body></html>");
    return 0;
}

