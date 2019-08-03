%{
    int charcount = 0 , linecount = 0, wordcount = 0;
%}

letter [^ \t\n]

%%

{letter}+ {wordcount++; charcount+=yyleng;}
.         {charcount++;}
\n        {linecount++; charcount++;}

%%

int main(){
    yylex();
    printf("Char count = %i\n", charcount);
    printf("Line count = %i\n", linecount);
    printf("Words count= %i\n", wordcount);
    return 0;
}
