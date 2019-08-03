#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "y.tab.h"

void yyerror(char* );

void
executeProgram(program* program, symrec** symTable, list* routineList){
    eval(program->statementList, symTable, routineList);
    // freeNode(program->statementList);
}


/**
 * Evaluation of constant parameter
 */
data
eval_constants(constant c, symrec** symTable){
    data res;
    basic b;
    b.type = c.type;
    switch (c.type) {
        case basic_int_value:
            b.i = c.value.int_value;
            break;
        case basic_float_value:
            b.f = c.value.float_value;
            break;
        case basic_boolean_value:
            b.b = c.value.bool_value;
            break;
        default:
            yyerror("new basicType added but not managed in the eval_constants funciton");
            exit(NOT_IMPLEMENTED_YET);
        break;
    }
    res.type = basic_dataType;
    res.b = b;
    return res;
}


/**
 *  this function evaluate all kind of expressions
 */
data
eval_expr(node expr, symrec** symTable, list* routineList){
    data res;
    switch (expr.operator) {
        case SEMICOLON:{
            // c'è sempre uno statement da valutare
            // valuto il primo
            eval(expr.op[0], symTable, routineList);
            if (expr.noperands > 1) {
            // valuto il secondo, non è possibile per la struttura
            // del parser averne più di due
                return eval(expr.op[1], symTable, routineList);
            }
            break;
        }
        case PRINT:{ // statement
            data e = eval(expr.op[0], symTable, routineList);
            printData(e);
            res.type = no_op;
            return res;
        }
        case EQUALS:{ // statement
            symrec* s = getSymbolFromIdentifier(expr.op[0]->value.id, symTable);
            // check if the variable is function
            routine* r = getRoutine(expr.op[0]->value.id.name, routineList);
            if (s == NULL && r == NULL) {
                yyerror("during assignment found unknown variable or function");
                exit(NO_SUCH_VARIABLE);
            }
            // else
            data res;
            if (s != NULL) {
                res = assignment(s, expr.op[1], symTable, routineList);
            }
            if (r != NULL) {
                // TODO check if this is a function or a procedure
                res = r_assignment(r, expr.op[1], symTable, routineList);
            }
            return res;
        }
        break;
        case WHILE:{ // statement
            // in ro con ambiente delta valuto <while
            // e to c, omega> --> c <c; while e do c,
            // omega> solo se in ro con ambiente delta 
            // posso valutare e a boleano vedi pag 54
            // semantica opearzionale
            data bool_guard = eval(expr.op[0], symTable, routineList);
            // here typechecking for correct return type of
            // bool_guard...must be boolean
            if (bool_guard.b.b == true) {
                eval(expr.op[1], symTable, routineList);
                // ho fatto il comando c, ora rimetto a valutazione la
                // stessa operazione
                treeNode* newWhile = opr(WHILE, 2, expr.op[0], expr.op[1]);
                eval(newWhile, symTable, routineList);
                // free(newWhile)
            }
            data res;
            res.type = no_op;
            return res;
        }
        break;
        case IF:{ // statement
            data bool_guard = eval(expr.op[0], symTable, routineList);
            // here typechecking for correct return type of
            // bool_guard...must be boolean
            if (bool_guard.b.b == true) {
                eval(expr.op[1], symTable, routineList);
            } else if (expr.noperands == 3) {	// se c'è il branch else
                eval(expr.op[2], symTable, routineList);
            }
            data res;
            res.type = no_op;
            return res;
        }
        break;
        case FOR:{ // statement
            symrec* s = getSymbolFromIdentifier(expr.op[0]->value.id, symTable);
            if (s == NULL) {
                yyerror("NO_SUCH_VARIABLE");
                exit(NO_SUCH_VARIABLE);
            }

            assignment(s, expr.op[1], symTable, routineList);
            identifier index;
            index.name = malloc(strlen(s->name) + 1);
            strcpy(index.name, s->name);
            data id = eval_identifier(index, symTable, routineList);
            // consider only LT
            data guard = eval(expr.op[2], symTable, routineList);
            data comparison = operation(LT, id, guard);
            // here happens typechecking
            if (comparison.b.b == true) {
                eval(expr.op[3], symTable, routineList);
                // incremento la variable, assume only integers
                treeNode* nextValue = constantNode(basic_int_value, id.b.i + 1);
                // assignment(s,nextValue,symTable);
                treeNode* newFor = opr(FOR, 4, expr.op[0], nextValue, expr.op[2],
                                        expr.op[3]);
                eval(newFor, symTable, routineList);
                // free(newFor);
                // free(nextValue);
            }
        }
        break;
        case UMINUS:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            return negate(e1);
        }
        case MINUS:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(MINUS, e1, e2);
        }
        break;
        case PLUS:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(PLUS, e1, e2);
        }
        break;
        case MULTIPLY:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(MULTIPLY, e1, e2);
        }
        break;
        case DIVIDE:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(DIVIDE, e1, e2);
        }
        break;
        case LT:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(LT, e1, e2);
        }
        break;
        case GT:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(GT, e1, e2);
        }
        break;
        case GE:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(GE, e1, e2);
        }
        break;
        case LE:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(LE, e1, e2);
        }
        break;
        case NE:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(NE, e1, e2);
        }
        break;
        case EQ:{
            data e1, e2;
            // typechecking happens here
            e1 = eval(expr.op[0], symTable, routineList);
            e2 = eval(expr.op[1], symTable, routineList);
            return operation(EQ, e1, e2);
        }
        break;
        default:
            yyerror("eval_expr new type not implemented");
            exit(NOT_IMPLEMENTED_YET);
        break;
    }
    return res;
}


/**
 * Evaluation of tree node with symbol table and routine list
 */
data
eval(treeNode* p, symrec** symTable, list* routineList){
    data d;
    if (p == NULL) {
        d.type = no_op;
        return d;
    }
    switch (p->type) {
        case const_type:
            return eval_constants(p->value.con, symTable);
        case identifier_type:
            return eval_identifier(p->value.id, symTable, routineList);
        case expression_type:
            return eval_expr(p->value.expr, symTable, routineList);
        case routine_type:
            return eval_routine(p->value.routine, symTable, routineList);
        case identifier_declaration:
            return eval_identifier_declaration(p->value.dec, symTable, routineList);
        default:
            yyerror("new nodeType added but not managed in the evaluation function");
            exit(NOT_IMPLEMENTED_YET);
            break;
    }
}


/**
 * print function of particular data given
 */
void
printData(data d){
    switch (d.type) {
        case basic_dataType:{
            switch (d.b.type) {
                case basic_int_value:
                    printf("%d\n", d.b.i);
                    break;
                case basic_float_value:
                    printf("%f\n", d.b.f);
                    break;
                case basic_boolean_value:
                    printf("%s\n", (d.b.b == true ? "TRUE" : "FALSE"));
                    break;
                case undef:
                default:
                    yyerror("printData unmanaged type in constant node");
                    exit(BUGGY_THE_CLOWN);
                break;
            }
        }
        break;
        case complex_dataType:
            yyerror("printing a complex type.. NOT_IMPLEMENTED_YET");
            exit(NOT_IMPLEMENTED_YET);
            break;
        case procedure_type:
            yyerror("printing a procedure.. unreachable state");
            exit(BUGGY_THE_CLOWN);
            break;
        case no_op:
            yyerror("evaluation returned no operation.. printing no_op.. unreachable state");
            exit(BUGGY_THE_CLOWN);
        break;
    }
}


// Integer arithmetic operation
int addInt(int n, int m){ return n + m; }
int subInt(int n, int m){ return n - m; }
int mulInt(int n, int m){ return n * m; }
int divInt(int n, int m){ return n / m; }

// Integer arithmetic operations
bool ltInt(int n, int m){ return (n < m) ? true : false; }
bool gtInt(int n, int m){ return (n > m) ? true : false; }
bool geInt(int n, int m){ return (n >= m) ? true : false; }
bool leInt(int n, int m){ return (n <= m) ? true : false; }
bool neInt(int n, int m){ return (n != m) ? true : false; }
bool eqInt(int n, int m){ return (n == m) ? true : false; }

// Float arithmetic operations
float addFloat(float n, float m){ return n + m; }
float subFloat(float n, float m){ return n - m; }
float mulFloat(float n, float m){ return n * m; }
float divFloat(float n, float m){ return n / m; }

// Float truth operations 
bool ltFloat(float n, float m){ return (n < m) ? true : false; }
bool gtFloat(float n, float m){ return (n > m) ? true : false; }
bool geFloat(float n, float m){ return (n >= m) ? true : false; }
bool leFloat(float n, float m){ return (n <= m) ? true : false; }
bool neFloat(float n, float m){ return (n != m) ? true : false; }
bool eqFloat(float n, float m){ return (n == m) ? true : false; }

// Boolean comparison operation
bool eqBool(bool n, bool m){ return (n == m) ? true : false; }


/**
 * Do the proper operation (calling the proper function above) between two data
 * We assume here type checking has already happened
 */
data
operation(int oper, data e1, data e2){
    data res;
    basic b;
    // pointer to a function that takes two int input and return an int
    int (*funIntPtr) (int, int);
    float (*funFloatPtr) (float, float);
    bool(*funBoolFloatPrt) (float, float);
    bool(*funBoolIntPrt) (int, int);
    bool(*funBoolPtr) (bool, bool);

    funIntPtr = NULL;
    funFloatPtr = NULL;
    funBoolFloatPrt = NULL;
    funBoolIntPrt = NULL;

    // I look for the function to be called
    switch (oper) {
        case MINUS:
        // *functionPtr = &subInt;
        switch (e1.b.type) {
            case basic_int_value:
                funIntPtr = subInt;
                break;
            case basic_float_value:
                funFloatPtr = subFloat;
                break;
            case basic_boolean_value:
                yyerror("boolean arithmetic operation are NOT_ALLOWED");
                exit(NOT_ALLOWED);
                break;
            case undef:
            default:
                yyerror("operation unmanaged type...");
                exit(BUGGY_THE_CLOWN);
                break;
        }
        break;
        case PLUS:
            switch (e1.b.type) {
            case basic_int_value:
                funIntPtr = addInt;
                break;
            case basic_float_value:
                funFloatPtr = addFloat;
                break;
            case basic_boolean_value:
                yyerror("boolean arithmetic operation are NOT_ALLOWED");
                exit(NOT_ALLOWED);
                break;
            case undef:
            default:
                yyerror("operation unmanaged type...");
                exit(BUGGY_THE_CLOWN);
                break;
        }
        break;
        case MULTIPLY:
            switch (e1.b.type) {
            case basic_int_value:
                funIntPtr = mulInt;
                break;
            case basic_float_value:
                funFloatPtr = mulFloat;
                break;
            case basic_boolean_value:
                yyerror("boolean arithmetic operation are NOT_ALLOWED");
                exit(NOT_ALLOWED);
                break;
            case undef:
            default:
                yyerror("operation unmanaged type...");
                exit(BUGGY_THE_CLOWN);
                break;
            }
        break;
        case DIVIDE:
            switch (e1.b.type) {
            case basic_int_value:
                funIntPtr = divInt;
                break;
            case basic_float_value:
                funFloatPtr = divFloat;
                break;
            case basic_boolean_value:
                yyerror("boolean arithmetic operation are NOT_ALLOWED");
                exit(NOT_ALLOWED);
                break;
            case undef:
            default:
                yyerror("operation unmanaged type...");
                exit(BUGGY_THE_CLOWN);
                break;
            }
        break;
        case LT:{
            switch (e1.b.type) {
                case basic_int_value:
                    funBoolIntPrt = ltInt;
                    break;
                case basic_float_value:
                    funBoolFloatPrt = ltFloat;
                    break;
                case basic_boolean_value:
                    yyerror("boolean arithmetic operation are NOT_ALLOWED");
                    exit(NOT_ALLOWED);
                    break;
                case undef:
                default:
                    yyerror("operation unmanaged type...");
                    exit(BUGGY_THE_CLOWN);
                break;
            }
        }
        break;
        case GT:{
            switch (e1.b.type) {
                case basic_int_value:
                    funBoolIntPrt = gtInt;
                    break;
                case basic_float_value:
                    funBoolFloatPrt = gtFloat;
                    break;
                case basic_boolean_value:
                    yyerror("boolean arithmetic operation are NOT_ALLOWED");
                    exit(NOT_ALLOWED);
                    break;
                case undef:
                default:
                    yyerror("operation unmanaged type...");
                    exit(BUGGY_THE_CLOWN);
                    break;
            }
        }
        break;
        case GE:{
            switch (e1.b.type) {
                case basic_int_value:
                    funBoolIntPrt = geInt;
                    break;
                case basic_float_value:
                    funBoolFloatPrt = geFloat;
                    break;
                case basic_boolean_value:
                    yyerror("boolean arithmetic operation are NOT_ALLOWED");
                    exit(NOT_ALLOWED);
                    break;
                case undef:
                default:
                    yyerror("operation unmanaged type...");
                    exit(BUGGY_THE_CLOWN);
                    break;
            }
        }
        break;
        case LE:{
            switch (e1.b.type) {
                case basic_int_value:
                    funBoolIntPrt = leInt;
                    break;
                case basic_float_value:
                    funBoolFloatPrt = leFloat;
                    break;
                case basic_boolean_value:
                    yyerror("boolean arithmetic operation are NOT_ALLOWED");
                    exit(NOT_ALLOWED);
                    break;
                case undef:
                default:
                    yyerror("operation unmanaged type...");
                    exit(BUGGY_THE_CLOWN);
                    break;
                }
        }
        break;
        case NE:{
            switch (e1.b.type) {
                case basic_int_value:
                    funBoolIntPrt = neInt;
                    break;
                case basic_float_value:
                    funBoolFloatPrt = neFloat;
                    break;
                case basic_boolean_value:
                    yyerror("boolean arithmetic operation are NOT_ALLOWED");
                    exit(NOT_ALLOWED);
                    break;
                case undef:
                default:
                    yyerror("operation unmanaged type...");
                    exit(BUGGY_THE_CLOWN);
                    break;
            }
        }
        break;
        case EQ:{
            switch (e1.b.type) {
                case basic_int_value:
                    funBoolIntPrt = eqInt;
                    break;
                case basic_float_value:
                    funBoolFloatPrt = eqFloat;
                    break;
                case basic_boolean_value:
                    funBoolPtr = eqBool;
                    break;
                case undef:
                default:
                    yyerror("operation unmanaged type...");
                    exit(BUGGY_THE_CLOWN);
                    break;
            }
        }
        break;
        default:
            yyerror("what now?");
            exit(BUGGY_THE_CLOWN);
            break;
    }
    // look for values to be used
    if(funIntPtr == NULL && funFloatPtr == NULL && funBoolIntPrt == NULL && 
            funBoolFloatPrt == NULL && funBoolPtr == NULL) {
        yyerror("no function found for the specified operation..");
        exit(BUGGY_THE_CLOWN);
    }
    res.type = basic_dataType;
    switch (e1.b.type) {
        case basic_int_value:{
            if (funIntPtr != NULL) {
                if (oper == EQUALS) {
                    // il risultato è booleano
                    res.b.type = basic_boolean_value;
                    res.b.b = funIntPtr(e1.b.i, e2.b.i);
                } else {
                    res.b.type = e1.b.type;	// inherithed
                    res.b.i = funIntPtr(e1.b.i, e2.b.i);
                }
            } else if (funBoolIntPrt != NULL) {
                // evaluating something like int < int or int == int
                res.b.type = basic_boolean_value;
                res.b.b = funBoolIntPrt(e1.b.i, e2.b.i);
            } else {
                yyerror("reached unexpected state..");
                exit(BUGGY_THE_CLOWN);
            }
        }
        break;
        case basic_float_value:{
            if (funFloatPtr != NULL) {
                if (oper == EQUALS) {
                    // il risultato è booleano
                    res.b.type = basic_boolean_value;
                    res.b.b = funFloatPtr(e1.b.f, e2.b.f);
                } else {
                    res.b.type = e1.b.type;	// inherithed
                    res.b.f = funFloatPtr(e1.b.f, e2.b.f);
                }
            } else if (funBoolFloatPrt != NULL) {
                res.b.type = basic_boolean_value;
                res.b.b = funBoolFloatPrt(e1.b.f, e2.b.f);
            } else {
                yyerror("reached unexpected state..");
                exit(BUGGY_THE_CLOWN);
            }
        }
        break;
        case basic_boolean_value:{
            if (funFloatPtr != NULL || funBoolFloatPrt != NULL || 
                    funBoolIntPrt != NULL || funIntPtr != NULL) {
                yyerror("reached unexpected state..");
                exit(BUGGY_THE_CLOWN);
            } else {
                res.b.type = e1.b.type;	// inherithed
                res.b.b = funBoolPtr(e1.b.b, e2.b.b);
            }
        }
        break;
        default:
            yyerror("no data found for the specified operation..");
            exit(BUGGY_THE_CLOWN);
    }
    return res;
}


/**
 * Negate the data given
 */
data
negate(data input){
    data res;
    res.type = input.type;
    basic b;
    switch (input.type) {
        case basic_dataType:{
            switch (input.b.type) {
                case basic_int_value:
                    b.type = input.b.type;
                    b.i = -input.b.i;
                    break;
                case basic_float_value:
                    b.type = input.b.type;
                    b.f = -input.b.f;
                    break;
                case basic_boolean_value:
                    b.type = input.b.type;
                    b.b = input.b.b == true ? false : true;
                    break;
                case undef:
                default:
                    yyerror("negate unmanaged type..");
                    exit(BUGGY_THE_CLOWN);
                break;
            }
        }
        break;
        case complex_dataType:
        default:
            yyerror("negating != basic data type is NOT_ALLOWED");
            exit(NOT_ALLOWED);
            break;
    }
    res.b = b;
    return res;
}


/**
 * Evaluation 
 */
data
eval_identifier_declaration(declarationNode decl, symrec** symbolTable, list* routineList){
    data res;
    res.type = no_op;
    symrec* s;
    s = getSym(decl.name, *symbolTable);
    if (s == 0) {
        s = createSym(decl.name, symbolTable);
        // insert
        switch (decl.t->dt) {
            case basic_dataType:{
                s->type = decl.t->dt;
                s->bType = decl.t->typeValue.bt;
                s->isCostant = decl.isCostant;
                if (s->isCostant) {
                    // assign the given expr
                    data e = eval(decl.expr, symbolTable, routineList);
                    // TODO typechecking tra il valore dichiarato e quello di e
                    switch (decl.t->typeValue.bt) {
                        case basic_int_value:
                            s->value.i = e.b.i;
                            break;
                        case basic_float_value:
                            s->value.f = e.b.f;
                            break;
                        case basic_boolean_value:
                            s->value.i = e.b.i;
                            break;
                        case undef:
                        default:
                            yyerror("negate unmanaged type..");
                            exit(BUGGY_THE_CLOWN);
                            break;
                    }
                }
            }
            break;
            case complex_dataType:
                yyerror("not implemented yet");
                exit(NOT_IMPLEMENTED_YET);
                break;
            default:
                yyerror("unmanaged data type in identifier_declaration");
                exit(NOT_IMPLEMENTED_YET);
        }
    } else {
        // TODO: provide concat to write out variable name
        yyerror("variable name already in use..");
        exit(NAME_ALREADY_IN_USE);
    }
    return res;
}


/**
 * Create a variable assignment
 */
data
spec_assignment(symrec* variable, data e, symrec** symTable, list* routineList){
    if (variable == NULL) {
        yyerror("variable not found");
        exit(NO_SUCH_VARIABLE);
    }
    data res;
    res.type = no_op;
    // assign the variable the given value
    switch (variable->type) {
        case basic_dataType:{
            switch (variable->bType) {
                case basic_boolean_value:
                    variable->value.b = e.b.b;
                    break;
                case basic_int_value:
                    variable->value.i = e.b.i;
                    break;
                case basic_float_value:
                    variable->value.f = e.b.f;
                    break;
                case undef:
                default:
                    yyerror("assignment unmanaged type..");
                    exit(BUGGY_THE_CLOWN);
                    break;
            }
        }
        break;
        case complex_dataType:{
            yyerror("assigning complex data type is NOT_IMPLEMENTED_YET");
            exit(NOT_IMPLEMENTED_YET);
        }
        break;
        default:{
            yyerror("assigning procedure or no_op to varibale");
            exit(BUGGY_THE_CLOWN);
        }
        break;
    }
    return res;
}


/**
 * Data assignment by variable and expression given
 */
data
assignment(symrec* variable, treeNode* expr, symrec** table, list* routineList){
    if (variable == NULL) {
        yyerror("variable not found");
        exit(NO_SUCH_VARIABLE);
    }
    // type checking happens here
    // control evaluate the given expression and check if has the same
    // type of s
    data e = eval(expr, table, routineList);
    return spec_assignment(variable, e, table, routineList);
}


/**
 * Changes the value pointed by r return value
 */
data
r_assignment(routine* r, treeNode* expr, symrec** symTable, list* routineList){
    if (r == NULL) {
        yyerror("routine not found");
        exit(NO_SUCH_VARIABLE);
    }

    data e = eval(expr, symTable, routineList);
    // type checking happens here
    r->returnValue = dataToDataPointer(e);
    data res;

    // is the last action that should be
    // executed in the program
    res.type = no_op;
    return res;
}


/**
 * Take an identifier and return its value
 */
data
eval_identifier(identifier identifierNode, symrec** symbolTable, list* routineList){
    // TODO: place a check for not initialized values
    data res;
    symrec* s = getSym(identifierNode.name, *symbolTable);
    if (s == 0) {
        // TODO: place variable name in output error
        yyerror("variable not found in environment");
        exit(NO_SUCH_VARIABLE);
    }
    res.type = s->type;
    switch (s->type) {
        case basic_dataType:{
            basic b;
                b.type = s->bType;
                switch (s->bType) {
                    case basic_boolean_value:
                    b.b = s->value.b;
                    break;
                case basic_int_value:
                    b.i = s->value.i;
                    break;
                case basic_float_value:
                    b.f = s->value.f;
                    //yyerror("accessing float variable NOT_IMPLEMENTED_YET");
                    //exit(NOT_IMPLEMENTED_YET);
                    break;
                case undef:
                default:
                    yyerror("eval_identifier unmanaged type..");
                    exit(BUGGY_THE_CLOWN);
                    break;
            }
            res.b = b;
            break;
        }
        case complex_dataType:
            // res.c = s->value.c;
            yyerror("assigning complex data type is NOT_IMPLEMENTED_YET");
            exit(NOT_IMPLEMENTED_YET);
            break;
        case no_op:
            break;
        case procedure_type:
            yyerror("assigning complex data type is NOT_IMPLEMENTED_YET");
            exit(NOT_IMPLEMENTED_YET);
            break;
    }
    return res;
}


/**
 * Evaluation of routine calls
 */
data
eval_routine(routineNode rout, symrec** symTable, list* routineList){
    data res;
    // recupera la routine e controlla se esiste
    routine* r = getRoutine(rout.name, routineList);
    if (r == NULL) {
        yyerror("accessing unexisting procedure or function..");
        exit(NO_SUCH_PROCEDURE);
    }

    // dalla routine prendi gli i parametri
    form* forms;
    forms = r->parameters;
    // dal nodo passato prendi gli argomenti
    actual* aes;
    aes = rout.args;

    // HERE HAPPENS type checking
    // we do minimal type checking on length - the rest is left as exercise
    int flen = formLength(forms);
    int alen = actLength(aes);
    if (flen != alen) {
        yyerror("args len is different from formal length");
    }

    // qui fai il bind tra parametri e argomenti, mettendoli nella symbol
    // TABLE
    // da ripassare a tutta la funzione di eval, usando il nodo expr come
    // funzione di partenza
    symrec* rSymrec;
    rSymrec = NULL;

    int i=0;
    for (i = 0; i < flen; i++) {
        form* f = getFormAtIndex(i, forms);
        if (f == NULL) {
            // TODO place here the number of forms already used
            yyerror("formal parameter list out of bound");
            exit(FORM_LIST_EXCEEDED);
        }
        actual* a = getActualAtIndex(i, aes);
        if (a == NULL) {
            // TODO place here the number of args already used
            yyerror("actual parameter list out of bound");
            exit(ARGS_LIST_EXCEEDED);
        }

        // eseguo dichiarazione e assegnazione nella symbol table che
        // userà la routine
        type* t = basicDec(f->bt);	
        // TODO: considered only basic type
        treeNode* dec = varDec(f->name, false, t);
        eval_identifier_declaration(dec->value.dec, &rSymrec, routineList);
        // cerca la var appena inserita e fai l'assignement
        identifier id;
        id.name = malloc(strlen(f->name) + 1);
        strcpy(id.name, f->name);
        symrec* tmp = getSymbolFromIdentifier(id, &rSymrec);
        // eval expr in current env, then assign it
        data e = eval(a->expr, symTable, routineList);
        spec_assignment(tmp, e, &rSymrec, routineList);
    }
    // eventually with return res = eval...
    eval(r->statementList, &rSymrec, routineList);

    if (r->type == procedure) {
        res.type = no_op;
    } else {
        // qui cerca nella routine, nel suo return value il valore,
        // puliscilo e resituicilo
        res = *r->returnValue;
        r->returnValue = NULL;
    }
    return res;
}
