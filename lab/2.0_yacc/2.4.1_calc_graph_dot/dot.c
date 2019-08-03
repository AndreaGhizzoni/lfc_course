#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calc.h"
#include "y.tab.h"

char** label = NULL;
char** graph = NULL;
int lcurr = 0;
int gcurr = 0;
int l_size = 1;
int g_size = 1;

// dot specification language
char* label_format = "\t%d [label=\"%s\"]\n";
char* graph_arrow_format = "\t%d -- %d\n";

// name of output dot file
char* graphName = "tree_graph";
/**
 * this is a graph counter: is increased by 1 every time the ex() function is 
 * called, this mean, for this grammar, every statement that finish with ';' is
 * a separated graph. In this situation the only way to create multiple dot file
 * is to concatenate the file with this counter
 */
int gcounter = 0;


/* interface for drawing (can be replaced by "real" graphic using GD or other) */
void graphInit (void);
void graphFinish();
void graphLabel (char* s, int uid);
void graphDrawArrow (int uid_from, int uid_to);

/* recursive drawing of the syntax tree */
void exNode (nodeType* p);

/*****************************************************************************/
/* main entry point of the manipulation of the syntax tree */
int 
ex( nodeType* p ){
    graphInit();
    exNode( p ); //recursive drawing
    graphFinish();
    return 0;
}

void 
exNode( nodeType* p ){
    if (!p) return;

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
        graphLabel(s, p->uid);
        return;
    }

    int k;
    //node has children - for each children draw arrow and then draw children
    for (k = 0; k < p->opr.nops; k++) {
        graphLabel(s, p->uid);
        graphDrawArrow(p->uid, p->opr.op[k]->uid);
        exNode (p->opr.op[k]);
    }
}

void 
graphLabel( char* s, int uid ){
    if(lcurr == l_size){
        l_size++;
        char** tmp = (char**) realloc(label, (l_size) * sizeof(char*));
        if(!tmp){
            printf("can't allocate more memory\n");
            exit(0);
        }else{
            label = tmp;
        }
    }
    int size = snprintf(NULL, 0, label_format, uid, s);
    char* slabel = (char*)malloc(size * sizeof(char));
    sprintf(slabel, label_format, uid, s);
    label[lcurr++] = slabel;
}

void 
graphDrawArrow( int uid_from, int uid_to ){
    if(gcurr == g_size){
        g_size++;
        char **tmp = (char **) realloc(graph, (g_size) * sizeof(char*));
        if(!tmp){
            printf("can't allocate more memory\n");
            exit(0);
        }else{
            graph = tmp;
        }
    }
    int size = snprintf(NULL, 0, graph_arrow_format, uid_from, uid_to);
    char * adjacent = (char*)malloc(size * sizeof(char));
    sprintf(adjacent,graph_arrow_format, uid_from, uid_to);
    graph[gcurr++]=adjacent;
}

/**
 * initializes label and graph
 * reset index for current label and current graph for the next call of ex()
 * reset counter of label vector size for the next call of ex()
 * reset counter of graph vector size for the next call of ex()
 */
void 
graphInit( void ){
    label = (char**)malloc(100 * sizeof(char*));
    graph = (char**)malloc(100 * sizeof(char*));

    //reset counter for malloc
    l_size = 1;
    g_size = 1;

    //reset counters for array position
    lcurr = 0;
    gcurr = 0;
}

/**
 * flush label and graph in a file or to console
 * free used memeory
 * increment graphcounter
 */
void 
graphFinish() {
    int size = snprintf(NULL, 0, "%s%d.dot", graphName, gcounter);
    char* filename =(char*)malloc((size) * sizeof(char));
    sprintf(filename,"%s%d.dot", graphName, gcounter);
    FILE* fp = fopen(filename,"w");
    if (fp == NULL) {
        fprintf(stderr, "Can't open output file %s!\n",filename);
        exit(1);
    }
    
    //syntax of Dot is graph filename {...}
    fprintf(fp,"graph %s{\n", filename);
    int i=0;
    for (i=0; i<gcurr; i++) {
        fprintf(fp,"%s\n", graph[i]);
        if(graph[i]!=NULL)
            free(graph[i]);
    }
    int j=0;
    for (j=0; j<lcurr; j++) {
        fprintf(fp,"%s\n", label[j]);
        if(label[j]!=NULL)
            free(label[j]);
    }
    fprintf(fp,"}\n");

    //closing
    fclose(fp);
    //increment counter for graph
    gcounter++;
}
