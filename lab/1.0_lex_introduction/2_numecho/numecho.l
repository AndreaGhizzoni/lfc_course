
%%
[0-9]+ {ECHO;}
. // do nothing

%%
int main(){
    yylex();
}

