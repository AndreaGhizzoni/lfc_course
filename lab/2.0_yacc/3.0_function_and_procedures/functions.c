#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "header.h"

extern symrec* table;
void yyerror(const char*);

/**
 * Initialize the tree node structure
 */
treeNode*
init(){
    treeNode* res;
    if((res = malloc(sizeof(treeNode))) == NULL) {
        yyerror("Can not allocate more memory");
        exit(OUT_OF_MEMORY);
    }
    return res;
}


/**
 * Initialize a tree node as constant value from expression read.
 */
treeNode*
constantNode(const basicType bt, ...){
    treeNode* res = init();
    va_list ap;
    va_start(ap, bt);
    constant c;
    c.type = bt;
    switch (c.type) {
        case basic_int_value:
            c.value.int_value = va_arg(ap, int);
            break;
        case basic_float_value:
            c.value.float_value = va_arg(ap, double);
            break;
        case basic_boolean_value:
            c.value.bool_value = (va_arg(ap, int) >= 1 ? true : false);
            break;
        case undef:
        default:
            yyerror("unmanaged type in constant node");
            exit(BUGGY_THE_CLOWN);
        break;
    }
    va_end(ap);
    res->type = const_type; // const_type is in nodeType enumerator
    res->value.con = c;
    return res;
}


/**
 * Initialize a node type as a variable declaration
 */
treeNode*
identifierNode(const char* varName){
    treeNode* res = init();
    identifier id;
    id.name = malloc(strlen(varName) + 1);
    strcpy(id.name, varName);
    res->value.id = id;
    res->type = identifier_type; // identifier_type is in nodeType enumerator
    return res;
}


/**
 * Initialize a node type as operation
 */
treeNode*
opr(int oper, int nops, ...){
    va_list ap;
    treeNode* node = init();
    if((node->value.expr.op = malloc(nops*sizeof(treeNode))) == NULL) {
        yyerror("opr - No memory left for allocation");
        exit(OUT_OF_MEMORY);
    }
    node->type = expression_type; // expression_type is in nodeType enumerator
    node->value.expr.operator = oper;
    node->value.expr.noperands = nops;
    va_start(ap, nops);
    int i;
    for (i = 0; i < nops; i++) {
        node->value.expr.op[i] = va_arg(ap, treeNode*);
    }
    va_end(ap);
    return node;
}


/**
 * Initialize a node type as function call
 */
treeNode*
fpCall(const char* name, actual* args){
    treeNode* node = init();
    routineNode rtn;
    rtn.name = malloc(strlen(name) + 1);
    strcpy(rtn.name, name);
    rtn.args = args;
    node->type = routine_type;
    node->value.routine = rtn;
    return node;
}


/**
 * Create a tree node as a variable declaration
 */
treeNode*
varDec(char* name, bool constant, type* dataType, ...){
    if( dataType == NULL )
        yyerror("data type reached is null");
    va_list ap;
    treeNode* node = init();
    node->type = identifier_declaration;
    node->value.dec.name = malloc(strlen(name) + 1);
    strcpy(node->value.dec.name, name);
    node->value.dec.isCostant = constant;
    node->value.dec.t = dataType;
    va_start(ap, dataType);
    node->value.dec.expr = va_arg(ap, treeNode *);
    va_end(ap);
    return node;
}


/**
 * Create a row into a symbol table
 */
symrec*
createSym(char const* varName, symrec** symbolTable){
    //char* variableName = (char*)malloc(sizeof(char)*strlen(varName)+1);
    //strcpy(variableName, varName);
    //return putSym(variableName, symbolTable);
    return putSym(varName, symbolTable);
}


/**
 * Put the new record into symbol table given
 */
symrec*
putSym(char const* identifier, symrec** symbolTable){
    symrec* ptr = malloc(sizeof(symrec));
    ptr->name = (char*)malloc(sizeof(char)*strlen(identifier)+1);
    strcpy(ptr->name, identifier);
    ptr->next = *symbolTable;
    *symbolTable = ptr;
    return ptr;
}


/**
 * Search if an identifier exists into symbol table given
 */
symrec*
getSym(char const* identifier, symrec* symTable){
    symrec* ptr;
    for( ptr=symTable; ptr!=(symrec*)0; ptr=ptr->next ){
        if( strcmp(ptr->name, identifier) == 0 )
            return ptr;
    }
    return NULL;
}


/**
 * Create a tree node as a array declaration
 */
type*
arrayDec( int size, type* t, basicType bt ){
    type* res = malloc(sizeof(type));
    res->size = size;
    if (t == NULL) {
        res->dt = basic_dataType;
        res->typeValue.bt = bt;
        res->t = NULL;
    } else {
        res->dt = complex_dataType;
        res->typeValue.ct = complex_array;
        res->t = t;
    }
    return res;
}


/**
 * Create a tree node as a basic type declaration
 */
type*
basicDec( basicType bt ){
    type* res = malloc(sizeof(type));
    res->size = 0;
    res->dt = basic_dataType;
    res->typeValue.bt = bt;
    res->t = NULL;
    return res;
}


/**
 *
 */
form*
formList( form* new, form** list ){
    form** tmp = &((*list)->next);
    while(*tmp != NULL) {
        tmp = &((*tmp)->next);
    }
    *tmp = new;
    return (*list);
}


/**
 * Create a new formal parameter
 * Implemented considering only basic types
 */
form*
newParam( const char* paramName, dataType dt, ... ){
    form* res = malloc(sizeof(form));
    res->name = malloc(strlen(paramName) + 1);
    strcpy(res->name, paramName);
    res->type = dt;
    va_list ap;
    va_start(ap, dt);

    switch (dt) {
        case basic_dataType: 
            (*res).bt = va_arg(ap, basicType);
            break;
        case complex_dataType:
            (*res).ct = va_arg(ap, complexType);;
            break;
        default:
            yyerror("unmanaged type in parameter list.");
            exit(BUGGY_THE_CLOWN);
    }
    va_end(ap);
    res->next = NULL;
    return res;
}

// void printFormList(form * lista){
// form * f;
// int i = 0;
// for(f=lista;f!=NULL; f=f->next){
// write_log(NULL,logInt("param %d",i++));
// write_log(NULL,logString("param name %s", f->name));
// }
// }

/**
 * next param is the return type, just basic yet
 * implemented thinking only about basic types
 */
routine*
newRoutine(const char* name, form* formals, treeNode* statements, ... ){
    routine* res = malloc(sizeof(routine));
    res->name = malloc(strlen(name) + 1);
    strcpy(res->name, name);
    res->parameters = formals;
    res->statementList = statements;
    va_list ap;
    va_start(ap, statements);
    type* t = va_arg(ap, type *);
    // switch (t->dataType) etc etc

    if( t == NULL || t->typeValue.bt == undef ){
        res->bt = undef;
        res->type = procedure;
    } else {
        res->type = function;
        res->bt = t->typeValue.bt;
    }
    va_end(ap);
    res->returnType = basic_dataType;
    return res;
}


/**
 * add a new routine list
 */
list*
addToList(routine* newRoutine, list** rList){
    list* l = malloc(sizeof(list));
    l->r = newRoutine;
    l->type = routine_list;
    if (rList == NULL) {
        return l;
    }
    l->next = *rList;
    *rList = l;
    return l;
}

// void printAllProcAndFun(list * rList){
// if(rList == NULL){
// write_log(NULL,"empty list");
// }
// list * l;
// for(l = rList; l!=NULL; l = l->next){
// write_log(NULL,logString("name %s", l->r->name));
// }
// }

/**
 * Create a new actual parameter associated an treeNode as expression
 */
actual*
newActual(treeNode* expr){
    actual* act = malloc(sizeof(actual));
    act->expr = expr;
    act->next = NULL;
    return act;
}


/**
 * Add to the actual parameter list the new actual parameter
 */
actual*
addToActList(actual* new, actual** list){
    actual** tmp = &((*list)->next);
    while(*tmp != NULL){
        tmp = &((*tmp)->next);
    }
    *tmp = new;
    return (*list);
}


/**
 * Return the actual parameter's length
 */
int
actLength(actual* args){
    int count = 0;
    if (args == NULL) {
        return count;
    }
    actual* temp;
    for (temp = args; temp != NULL; temp = temp->next) {
        count++;
    }
    return count;
}


/**
 * Return the formal parameter's length
 */
int
formLength(form* forms){
    int count = 0;
    if (forms == NULL) {
        return count;
    }
    form* temp;
    for (temp = forms; temp != NULL; temp = temp->next) {
        count++;
    }
    return count;
}


/**
 * Return the routine* from routine list given with the same name as name
 * parameter
 */
routine*
getRoutine(const char* name, list* routineList){
    if (routineList == NULL) {
        return NULL;
    }
    // TODO typecheking about routineList being really a list of type
    // routine
    routine* ptr;
    list* tmp;
    // write_log(NULL,logString("looking for %s",name));
    for(tmp = routineList; tmp != NULL; tmp = tmp->next) {
        ptr = tmp->r;
        // write_log(NULL,logString("name found %s",ptr->name));
        if (strcmp(ptr->name, name) == 0) {
            return ptr;
        }
    }
    return NULL;
}


/**
 * Return the formal parameter from given list at given index 
 */
form*
getFormAtIndex(int index, form* list){
    form* tmp;
    int i = 0;
    for (tmp = list; tmp != NULL; tmp = tmp->next) {
        if (i == index) {
            return tmp;
        }
        i++;
    }
    return NULL;
}


/**
 * Return the actual parameter from given list at given index 
 */
actual*
getActualAtIndex(int index, actual* list){
    actual* tmp;
    int i = 0;
    for (tmp = list; tmp != NULL; tmp = tmp->next) {
        if (i == index) {
            return tmp;
        }
        i++;
    }
    return NULL;
}


/**
 * Return a record from Symbol Table with identifier given
 */
symrec*
getSymbolFromIdentifier(identifier identifierNode, symrec** symbolTable){
    symrec* s = getSym(identifierNode.name, *symbolTable);
    if (s == 0) {
        // TODO: place variable name in output error
        // yyerror("VARIABLE NOT FOUND");
        // exit(NO_SUCH_VARIABLE);
    }
    return s;
}


/**
 * Cast of data to data*
 * makes a pointer out of the data type
 * simplified cause we do not manage complex return type, from functions...
 */
data*
dataToDataPointer(data d){
    data* res = malloc(sizeof(data));
    // TODO: check if we have a pointer or null
    res->type = d.type;
    switch (d.type) {
        case basic_dataType:{
            basic bRes;
            switch (d.b.type) {
                case basic_int_value:
                    bRes.type = basic_int_value;
                    bRes.i = d.b.i;
                    break;
                case basic_float_value:
                    bRes.type = basic_float_value;
                    bRes.f = d.b.f;
                    //yyerror("BOOOOM you get so far, how? float is not implemented");
                    //exit(NOT_IMPLEMENTED_YET);
                    break;
                case basic_boolean_value:
                    bRes.type = basic_boolean_value;
                    bRes.b = d.b.b;
                    break;
                default:
                    yyerror("new data type not implemented in dataToDataPointer");
                    exit(NOT_IMPLEMENTED_YET);
            }
            res->b = bRes;
            break;
        }
        case complex_dataType:
            yyerror("complex_dataType not implemented in dataToDataPointer");
        default:
            yyerror("unmanaged data copy");
            exit(BUGGY_THE_CLOWN);
    }
    return res;
}
