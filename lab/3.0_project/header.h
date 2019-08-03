//157507
#ifndef HEADER_H
#define HEADER_H

#define true 1
#define false 0

// error DEFINITIONS
#define NOT_IMPLEMENTED_YET -1
#define OUT_OF_MEMORY -2
#define BUGGY_THE_CLOWN -3
#define NOT_ALLOWED -3
#define NAME_ALREADY_IN_USE -4
#define NO_SUCH_VARIABLE -5
#define NO_SUCH_PROCEDURE -6
#define FORM_LIST_EXCEEDED -7
#define ARGS_LIST_EXCEEDED -8

// ============================= ENUMERATORS ===================================
/* node type */
typedef enum { 
    const_type, 
    identifier_type,
    expression_type, 
    routine_type, 
    identifier_declaration 
} nodeType;	

/* basic type  */
typedef enum { 
    undef, 
    basic_int_value,
    basic_float_value,
	basic_boolean_value 
} basicType;

/* complex type */
typedef enum { 
    complex_array, 
    complex_structure 
} complexType;

/* data type  */
typedef enum { 
    basic_dataType, 
    complex_dataType, 
    procedure_type,
	no_op 
} dataType;

/* routine type */
typedef enum { 
    function, 
    procedure 
} routineType;

/* list content */
typedef enum { routine_list } listContent;


// ============================= BASIC TYPE DEFINITIONS ========================
/* boolean */
typedef int bool;

/* basic */
typedef struct basic {
    basicType type;
    union {
        int i;
        float f;
        bool b;
    };
} basic;

/* array */
typedef struct array {
    basicType type;
    union { 
        int  i;
        //float f;
        bool b;
    };
} array;

/* complex */
typedef struct complex {
    complexType type;
    union {
        int i;
        //float f;
        bool b;
    };
} complex;

/* type */
typedef struct type {
    dataType dt;
    int size;
    union {
        basicType bt;
        complexType ct;
    } typeValue;
    struct type* t;	// is the next type
} type;

/** type array Functions */
type* arrayDec(int, type*, basicType);
type* basicDec(basicType);


// ============================= SYMBOL TABLE TYPE DEFINITIONS =================
/**
 * a symbol in the symbol table - name is the name of the symbol - type
 * indicates the data type stored by the symbol 
 */
typedef struct symrec {
    char* name;
    dataType type;
    basicType bType;
    complexType cType;
    bool isCostant;
    union {
        int i;
        float f;
        bool b;
        complex c;
    } value;
    struct symrec* next;
} symrec;

/** symrec FUNCTIONS */
symrec* getSym(char const*, symrec*);
symrec* createSym(char const*, symrec**);
symrec* putSym(char const*, symrec**);


// ============================= TREE NODE TYPE DEFINITIONS ====================
/** constants are stored in constant node type union */
typedef struct constant {
    basicType type;
    union {
        int int_value;
        bool bool_value;
        float float_value;
    } value;
} constant;

/** identifier for node union */
typedef struct identifier {
    char* name;
} identifier;

/** node */
typedef struct {
    int operator;	      /* operator */
    int noperands;	      /* number of operands */
    struct treeNode** op; /* operands */
} node;

/** declaration node */
typedef struct declarationNode {
    char* name;
    bool isCostant;
    type* t;
    struct treeNode *expr;
} declarationNode;

/* routine node */
typedef struct routineNode {
    char* name;
    struct actual* args;
} routineNode;

/* tree node */
typedef struct treeNode {
    nodeType type; /* type of node, tells us the value contained in the union */
    union {
        constant con;         /* constants */
        identifier id;        /* identifiers */
        node expr;	          /* can be the same under the name of operator */
        declarationNode dec;
        routineNode routine;  /* routine or procedure */
    } value;
} treeNode;

treeNode* init();
treeNode* constantNode(const basicType, ...);
treeNode* identifierNode(const char*);
treeNode* opr(int oper, int nops, ...);
treeNode* fpCall(const char*, struct actual*);
treeNode* varDec(char*, bool constant, type* dataType, ...);


// ============================= FORMAL PARAMETERS TYPE DEFINITIONS ============
/* form */
typedef struct form {
    char* name;
    dataType type;
    union {
        basicType bt;
        complexType ct;
    };
    struct form* next;
} form;

form* newParam(const char*, dataType, ...);
form* formList(form*, form**);
int formLength(form*);
void printFormList(form*);
form* getFormAtIndex(int, form*);
form* formList(form*, form**);


// ============================= ACTUAL PARAMETERS TYPE DEFINITIONS ============
/* actual */
typedef struct actual {
    treeNode       *expr;
    struct actual  *next;
} actual;

actual* newActual(treeNode*);
actual* addToActList(actual*, actual**);
int actLength(actual*);
actual* getActualAtIndex(int, actual*);


// ============================= ROUTINE TYPE DEFINITIONS ======================
/* routine */
typedef struct routine {
    char* name;
    routineType type;
    dataType returnType;
    union {
        basicType bt;
        complexType ct;
    };
    struct data* returnValue; // so that if it is NULL is clean
    form* parameters;
    treeNode* statementList;
    symrec* env; // TODO: use it for evaluation or remove it
} routine;

routine* newRoutine(const char*, form*, treeNode*, ...);

/* list */
typedef struct list {
    listContent type;
    union {
        routine* r;
    };
    struct list* next;
} list;

routine* getRoutine(const char*, list*);
list* addToList(routine*, list**);
//void printAllProcAndFun(list* rList);


// ============================= PROGRAM TYPE DEFINITIONS ======================
/* program */
typedef struct program {
    treeNode* statementList;
    list* routineList;
    struct symrec* symtable;
} program;

void executeProgram(program*, symrec**, list*);
//void freeNode(treeNode*);


// ============================= DATA TYPE DEFINITIONS =========================
/* used in interpreter.c to hold result evaluation */
typedef struct data {
    dataType type;
    union {
        complex c;
        basic b;
    };
} data;

data eval(treeNode* p, symrec**, list*);
data eval_expr(node, symrec**, list*);
data eval_routine(routineNode, symrec**, list*);
data eval_constants(constant, symrec**);
data eval_identifier(identifier, symrec**, list*);
data eval_identifier_declaration(declarationNode, symrec**, list* routineList);
// for already evaluated data types
// assigns data to symrec passed, does not use yet the routineList nor the 
// symrec table
data spec_assignment(symrec*, data, symrec**, list*);
// general one, evaluates treeNode and assign it to symrec, using the given 
// symrec table
data assignment(symrec*, treeNode*, symrec**, list*);
data r_assignment(routine*, treeNode*, symrec**, list*);
data operation(int oper, data e1, data e2);
data* dataToDataPointer(data);
void printData(data);
data negate(data);

symrec* getSymbolFromIdentifier(identifier, symrec**);
#endif
