%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include "calc.h"
    
    /* prototypes */
    nodeType* opr(int oper, int nops, ...);
    nodeType* id(int i);
    nodeType* con(int value);
    void freeNode(nodeType* p);
    int ex(nodeType* p);

    int yylex(void);
    void yyerror(const char* s);

    int sym[26]; /* symbol table */
    int uid = 0;
%}

%error-verbose

/* these are the possible values that a TOKEN (from lex) can have */
%union {
    int iValue;      /* integer value */
    char sIndex;     /* symbol table index */
    nodeType *nPtr;  /* node pointer */
}

%type <nPtr> stmt expr stmt_list

%token <iValue> INTEGER         
%token <sIndex> VARIABLE
%token WHILE IF PRINT

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS
%nonassoc IFX
%nonassoc ELSE

%%

program: function {exit(0);}
       ;

function: function stmt {ex($2); freeNode($2);}
        | /* NULL */
        ;

stmt: ';'                             {$$ = opr(';', 2, NULL, NULL);}
    | expr ';'                        {$$ = $1;}
    | PRINT expr ';'                  {$$ = opr(PRINT,1,$2);}
    | VARIABLE '=' expr ';'           {$$ = opr('=',2,id($1),$3);}
    | WHILE '(' expr ')' stmt         {$$ = opr(WHILE,2,$3,$5);}
    | IF '(' expr ')' stmt %prec IFX  {$$ = opr(IF,2,$3,$5);}
    | IF '(' expr ')' stmt ELSE stmt  {$$ = opr(IF,3,$3,$5,$7);}
    | '{' stmt_list '}'               {$$ = $2;}
    ;


stmt_list: stmt             {$$ = $1;}
         | stmt_list stmt   {$$ = opr(';', 2, $1, $2);}
         ;

expr: INTEGER               {$$ = con($1);} //manage constants
    | VARIABLE              {$$ = id($1);}  //manage variables - namely an IDENTIFIER
    | '-' expr %prec UMINUS {$$ = opr(UMINUS,1,$2);}
    | expr '+' expr         {$$ = opr('+',2,$1,$3);}
    | expr '-' expr         {$$ = opr('-',2,$1,$3);}
    | expr '*' expr         {$$ = opr('*',2,$1,$3);}
    | expr '/' expr         {$$ = opr('/',2,$1,$3);}
    | expr '<' expr         {$$ = opr('<',2,$1,$3);}
    | expr '>' expr         {$$ = opr('>',2,$1,$3);}
    | expr GE expr          {$$ = opr(GE,2,$1,$3);}
    | expr LE expr          {$$ = opr(LE,2,$1,$3);}
    | expr NE expr          {$$ = opr(NE,2,$1,$3);}
    | expr EQ expr          {$$ = opr(EQ,2,$1,$3);}
    | '(' expr ')'          {$$ = $2;}
    ;

%%

/**
 * create constant node Type
 */
nodeType* 
con(int value){
    nodeType *p;
    /* allocate node space in memory */
    if((p=malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    /* copy information */
    p->uid = uid++;
    p->type = typeCon;
    p->con.value = value;
    
    return p;
}


/**
 * create an id node Type
 */
nodeType*
id (int i){
    nodeType *p;
    if((p=malloc(sizeof(nodeType)))==NULL){
        yyerror("out of memory");
    }
    p->uid = uid++;
    p->type = typeId;
    p->id.i=i;
    
    return p;
}


/**
 * create an operation node Type
 */
nodeType* 
opr(int oper, int nops, ...){
    va_list ap; /* (ap = argument pointer) va_list is used to declare a variable
                 which, from time to time, is referring to an argument*/
    nodeType *p;
    int i;
    
    if ((p = malloc(sizeof(nodeType))) == NULL){
        yyerror("out of memory");
    }
    if((p->opr.op = malloc(nops*sizeof(nodeType)))== NULL){
        yyerror("out of memory");
    }
    p->uid = uid++;
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);/* initialize the sequence, makes ap point to the first
                        anonymous argument. Must call it once before reading all
                        the parameters*/
    for(i = 0; i<nops;i++){
        p->opr.op[i]=va_arg(ap,nodeType*); /*every time va_arg is called it returns
                                            an argument and moves the pointer forward
                                            to the next argument. It uses a type name
                                            to decide what kind of type to return and
                                            how much to move forward the pointer.
                                            */
        p->opr.op[i]->uid = uid++;
    }
    va_end(ap); /* MUST be called BEFORE THE FUNCTION TERMINATES. it provides
                 the necessary cleaning functions.*/
    return p;
}


/**
 * we keep memory clean
 */
void 
freeNode(nodeType *p){
    int i;
    if(!p) return;
    if(p->type == typeOpr){
        for(i=0;i<p->opr.nops; i++){
            freeNode(p->opr.op[i]);
        }
        free(p->opr.op);
    }
    free(p);
}


/**
 * yyerror stuff
 */
void 
yyerror(const char *s){
    fprintf(stdout,"%s\n",s);
}

// MAIN
int 
main(){
    yyparse();
    return 0;
}
