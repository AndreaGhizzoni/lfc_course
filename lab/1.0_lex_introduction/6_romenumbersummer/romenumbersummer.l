%{
    // variable declaration declaration
    int c = 0;
%}

%%

[I] { c+=1; }
[V] { c+=5; }
[X] { c+=10; }
[L] { c+=50; }
[C] { c+=100; }
[D] { c+=500; }
[M] { c+=1000; }
.
\n

%%

int main(){
    yylex();
    printf("sum = %i\n", c);
    return 0;
}

