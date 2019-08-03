%{
    #include <string.h>
    FILE* pFile;
    void rand_str(char*, size_t);
%}

%start VAR

%%

(int|real) {
    fputs(yytext, pFile);
    BEGIN VAR;
}

<VAR>[a-zA-Z0-9]+ { 
    int l = strlen(yytext);
    char* dst = malloc(sizeof(char)*l);
    rand_str(dst, l);
    fputs(dst, pFile);

    BEGIN 0;
}

\n {
    fputs("\n", pFile);
}

. {
    fputs(yytext, pFile);
}

%%

void 
rand_str(char *dest, size_t length) {
    char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    while (length-- > 0) {
        size_t index = (double)rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int 
main(){
    pFile = fopen("output", "w");
    yylex();
    fclose(pFile);
    return 0;
}

