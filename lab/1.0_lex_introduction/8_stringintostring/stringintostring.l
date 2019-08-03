%{
    // variable declaration
    // "\\".*\\"" {ECHO;}
%}

%%

\"(\\.|[^\\"])*\" {ECHO;}
.


%%

int main(){
    yylex();
    return 0;
}

