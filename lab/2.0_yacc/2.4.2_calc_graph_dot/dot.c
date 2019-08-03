#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calc.h"
#include "y.tab.h"

// definition of list
struct list_el {
    char* val;
    struct list_el* next;
};
typedef struct list_el item;

// LIST DECLARATION
item* label_list_curr = NULL; // current element of label list
item* label_list_head = NULL; // head element of label list
item* graph_list_curr = NULL; // current element of graph list
item* graph_list_head = NULL; // head element of graph list

// dot specification language
char* label_format = "\t%d [label=\"%s\"]\n";
char* graph_arrow_format = "\t%d -- %d\n";

// name of output dot file. Used in flushLists()
char* graphName = "tree_graph.dot";

char* formatLabel (char* s, int uid);
char* formatGraph (int uid_from, int uid_to);

void initLists();
void appendLabel(char*);
void appendGraph(char*);
void flushLists();

/* recursive drawing of the syntax tree */
void exNode (nodeType* p);

/*****************************************************************************/
/* main entry point of the manipulation of the syntax tree */
int 
ex( nodeType* p ){
    initLists(); // called several time, I know. Control statement in function
    exNode( p ); //recursive drawing
    if(!p) flushLists();
    return 0;
}

void 
exNode( nodeType* p ){
    if( !p ) return;

    char* s = malloc(sizeof(char*)); // node text to use as label in dot
    switch(p->type) { //define label in s
        case typeCon:
            sprintf (s, "c(%d)", p->con.value); 
            break;
        case typeId:
            sprintf (s, "id(%c)", p->id.i+'A'); 
            break;
        case typeOpr:
            switch(p->opr.oper){
                case WHILE:     sprintf(s, "while"); break;
                case IF:        sprintf(s, "if");    break;
                case PRINT:     sprintf(s, "print"); break;
                case ';':       sprintf(s, "[;]");   break;
                case '=':       sprintf(s, "[=]");   break;
                case UMINUS:    sprintf(s, "[_]");   break;
                case '+':       sprintf(s, "[+]");   break;
                case '-':       sprintf(s, "[-]");   break;
                case '*':       sprintf(s, "[*]");   break;
                case '/':       sprintf(s, "[/]");   break;
                case '<':       sprintf(s, "[<]");   break;
                case '>':       sprintf(s, "[>]");   break;
                case GE:        sprintf(s, "[>=]");  break;
                case LE:        sprintf(s, "[<=]");  break;
                case NE:        sprintf(s, "[!=]");  break;
                case EQ:        sprintf(s, "[==]");  break;
            }
            break;
    }
    
    //node is leaf
    if (p->type == typeCon || p->type == typeId || p->opr.nops == 0) {
        appendLabel(formatLabel(s, p->uid));
        return;
    }

    int k;
    //node has children - for each children draw arrow and then draw children
    for (k = 0; k < p->opr.nops; k++) {
        appendLabel(formatLabel(s, p->uid));
        appendGraph(formatGraph(p->uid, p->opr.op[k]->uid));
        exNode (p->opr.op[k]);
    }
}

/**
 * Create a string formatted as declared as label_format
 */
char* 
formatLabel( char* s, int uid ){
    int size = snprintf(NULL, 0, label_format, uid, s);
    char* slabel = (char*)malloc(size * sizeof(char));
    sprintf(slabel, label_format, uid, s);
    return slabel;
}

/**
 * Create a string formatted as declared as graph_arrow_format
 */
char*
formatGraph( int uid_from, int uid_to ){
    int size = snprintf(NULL, 0, graph_arrow_format, uid_from, uid_to);
    char * adjacent = (char*)malloc(size * sizeof(char));
    sprintf(adjacent,graph_arrow_format, uid_from, uid_to);
    return adjacent;
}

/**
 * This function initialize label and graph head and curr pointer.
 * This function is called several time from ex(), so to prevent multiple
 * initialization, they are wrapped into if control statement.
 */
void 
initLists(){
    if(!label_list_head){
        label_list_head = (item*)malloc(sizeof(item));
        label_list_head->val = NULL;
        label_list_head->next = label_list_head;
        label_list_curr = label_list_head;
    }
    if(!graph_list_head){
        graph_list_head = (item*)malloc(sizeof(item));
        graph_list_head->val = NULL;
        graph_list_head->next = graph_list_head;
        graph_list_curr = graph_list_head;
    }
}

/**
 * Append a given char* at the end of label list if and only if char* != NULL
 */
void
appendLabel(char* str ){
    if(str){
        item* newitem = (item*)malloc(sizeof(item));
        newitem->val = str;
        newitem->next = label_list_head;
        label_list_curr->next = newitem;
        label_list_curr = newitem;
    }
}

/**
 * Append a given char* at the end of graph list if and only if char* != NULL
 */
void
appendGraph(char* str){
    if(str){
        item* newitem = (item*)malloc(sizeof(item));
        newitem->val = str;
        newitem->next = graph_list_head;
        graph_list_curr->next = newitem;
        graph_list_curr = newitem;
    }
}

/**
 * flush label and graph list into file.dot.
 */
void 
flushLists() {
    int size = snprintf(NULL, 0, "%s", graphName);
    char* filename =(char*)malloc((size) * sizeof(char));
    sprintf(filename,"%s.dot", graphName);
    FILE* fp = fopen(filename,"w");
    if (fp == NULL) {
        fprintf(stderr, "Can't open output file %s!\n",filename);
        exit(1);
    }
    
    //syntax of Dot is graph filename {...}
    fprintf(fp,"graph %s{\n", "tree");
    if(graph_list_head->next){
        item* curr_graph = graph_list_head->next;
        while(curr_graph != graph_list_head){
            if(curr_graph->val)
                fprintf(fp, "%s\n", curr_graph->val);
            curr_graph = curr_graph->next;
        }
    }
    if(label_list_head->next){
        item* curr_label = label_list_head->next;
        while(curr_label != label_list_head){
            if(curr_label->val)
                fprintf(fp, "%s\n", curr_label->val);
            curr_label = curr_label->next;
        }
    }
    fprintf(fp,"}\n");

    //closing
    fclose(fp);
}
