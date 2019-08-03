%{
    int sum = 0;
%}

%%

[0-9]+ {sum+=atoi(yytext);}
.   // do nothing

%%

int main(){
    yylex();
    printf("Sum = %i\n", sum);
    return 0;
}

