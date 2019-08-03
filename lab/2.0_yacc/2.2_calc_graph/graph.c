#include <stdio.h>
#include <string.h>

#include "calc.h"
#include "y.tab.h"

/* interface for drawing */
#define lmax 200
#define cmax 200

char graph[lmax][cmax]; /* array for ASCII-Graphic */
int graphNumber = 0;

int del = 1; /* distance of graph columns */
int eps = 3; /* distance of graph lines */

/* interface for drawing (can be replaced by "real" graphic using GD or other) */
void graphInit (void);
void graphFinish();
void graphBox (char* s, int* w, int* h);
void graphDrawBox (char* s, int c, int l);
void graphDrawArrow (int c1, int l1, int c2, int l2);

/* recursive drawing of the syntax tree */
void exNode (nodeType* p, int c, int l, int* ce, int* cm);

/*****************************************************************************/

/* main entry point of the manipulation of the syntax tree */
int 
ex( nodeType* p ) {
    int rte, rtm;
    
    graphInit (); //initializes a matrix 200 * 200 (lmax * cmax)
    exNode (p, 0, 0, &rte, &rtm); //recursive drawing
    graphFinish();
    return 0;
}

/*c----cm---ce---->                       drawing of leaf-nodes
 l leaf-info
 */

/*c---------------cm--------------ce----> drawing of non-leaf-nodes
 l            node-info
 *                |
 *    -------------     ...----
 *    |       |               |
 *    v       v               v
 * child1  child2  ...     child-n
 *        che     che             che
 *cs      cs      cs              cs
 *
 */

void 
exNode( nodeType *p,
        int c, int l,        /* start column and line of node */
        int *ce, int *cm){   /* resulting end column and mid of node */
    int w, h;           /* node width and height */
    char *s;            /* node text */
    int cbar;           /* "real" start column of node (centred above subnodes) */
    int k;              /* child number - cycle index*/
    int che, chm;       /* end column and mid of children */
    int cs;             /* start column of children */
    char word[20];      /* extended node text */
    
    if (!p) return;
    
    strcpy (word, "???"); /* should never appear */
    s = word;
    switch(p->type) {
        case typeCon: sprintf (word, "c(%d)", p->con.value); break; //stores the string in the array word
        case typeId:  sprintf (word, "id(%c)", p->id.i + 'A'); break;
        case typeOpr:
            switch(p->opr.oper){
                case WHILE:     s = "while"; break;
                case IF:        s = "if";    break;
                case PRINT:     s = "print"; break;
                case ';':       s = "[;]";     break;
                case '=':       s = "[=]";     break;
                case UMINUS:    s = "[_]";     break;
                case '+':       s = "[+]";     break;
                case '-':       s = "[-]";     break;
                case '*':       s = "[*]";     break;
                case '/':       s = "[/]";     break;
                case '<':       s = "[<]";     break;
                case '>':       s = "[>]";     break;
                case GE:        s = "[>=]";    break;
                case LE:        s = "[<=]";    break;
                case NE:        s = "[!=]";    break;
                case EQ:        s = "[==]";    break;
            }
            break;
    }
    
    /* construct node text box */
    graphBox (s, &w, &h);
    cbar = c; /* cbar = real start of colum of node,
               centered wrt subnodes is equal
               to parameter c which is the starting column - look at picture above
               by calling recursively this function we indent the starting of the
               column with respect to the nodes we want to call
               */
    *ce = c + w; /* resulting end column of outputted box = 
                  starting column (c) + computed witdh (h)*/
    *cm = c + w / 2; /* mid of node is the occupied space /2
                      we take it simple*/
    
    /* node is leaf */
    if (p->type == typeCon || p->type == typeId || p->opr.nops == 0) {
        /* eg. constants, identifiers, print operation*/
        //recall cbar is the real start of colum node
        //l is the line of the node
        graphDrawBox (s, cbar, l);
        return;
    }
    
    /* node has children */
    cs = c;
    for (k = 0; k < p->opr.nops; k++) {
        exNode (p->opr.op[k], cs, l+h+eps, &che, &chm);
        cs = che;
        /*
         cs = end of previous node,
         is changed casuse we pass a pointer to memory, 
         thus we can hold the shift caused by children nodes
         */
    }
    
    /* total node width 
     w = computed witdh by the graphBox function by this node
     che - c = end colum of last child - starting colum --> space occupied by child nodes
     w < che - c = if space of this node < space occupied by nodes then
     */
    if (w < che - c) {
        cbar += (che - c - w) / 2; //centering the column start cbar + half of the space described above
        *ce = che; //modifying column end, with the resulting end of children
        *cm = (c + che) / 2; //modifying mid node start
    }
    
    /* draw node 
     starting by the computed mid point cbar, see snipped above
     */
    graphDrawBox (s, cbar, l);
    
    /* draw arrows (not optimal: children are drawn a second time) */
    cs = c; // cs = start of column children
    for (k = 0; k < p->opr.nops; k++) {
        exNode (p->opr.op[k], cs, l+h+eps, &che, &chm);
        /*
         cm - mid node pointer
         l+h - l is the line + h which is the calculated height
         chm - mid of the children, resulting from the previous function call
         (l + h) + space between lines delimits the box
         this four parameters delimits the rectangle
         */
        graphDrawArrow (*cm, l+h, chm, l+h+eps-1);
        cs = che;
    }
}

/*
 test if nodes are drawed correctly
 l - line of the node
 c - column lenght?
 
 */
void 
graphTest( int l, int c ){   
    int ok;
    ok = 1;
    if (l < 0) ok = 0; // line is less than zero, computation error?
    if (l >= lmax) ok = 0; // out of printable bounds, above first line starts
    if (c < 0) ok = 0; //coulumn is less than zero, before first column starts
    if (c >= cmax) ok = 0; //out of printable bounds
    if (ok) return;
    printf ("\n+++error: l=%d, c=%d not in drawing rectangle 0, 0 ... %d, %d",
            l, c, lmax, cmax);
    return;
    //exit (1);
}

void 
graphInit( void ){
    int i, j;
    for (i = 0; i < lmax; i++) {
        for (j = 0; j < cmax; j++) {
            graph[i][j] = ' ';
        }
    }
}

void 
graphFinish(){
    int i, j;
    for (i = 0; i < lmax; i++) {
        for (j = cmax-1; j > 0 && graph[i][j] == ' '; j--);
        graph[i][cmax-1] = 0;
        if (j < cmax-1) graph[i][j+1] = 0;
        if (graph[i][j] == ' ') graph[i][j] = 0;
    }
    for (i = lmax-1; i > 0 && graph[i][0] == 0; i--);
    printf ("\n\nGraph %d:\n", graphNumber++);
    for (j = 0; j <= i; j++) printf ("\n%s", graph[j]);
    printf("\n");
}

void 
graphBox( char* s, int* w, int* h ) {
    /* construct the box of the node, 
       just take the len of the string + len of delimiter
       and height = 1 */
    *w = strlen (s) + del;
    *h = 1;
}

/**
 char *s - string to be printed as node label
 int c - starting column
 int l - line of the node
 */
void 
graphDrawBox( char* s, int c, int l ){
    int i;
    graphTest (l, c+strlen(s)-1+del);
    for (i = 0; i < strlen (s); i++) {
        //place a char of s every space in the "matrix"
        graph[l][c+i+del] = s[i];
    }
}

void 
graphDrawArrow( int c1, int l1, int c2, int l2 ){
    int m;
    graphTest (l1, c1);
    graphTest (l2, c2);
    m = (l1 + l2) / 2; //mid point in the line
   
    while (l1 != m) { graph[l1][c1] = '|'; if (l1 < l2) l1++; else l1--; }
    while (c1 != c2) { graph[l1][c1] = '-'; if (c1 < c2) c1++; else c1--; }
    while (l1 != l2) { graph[l1][c1] = '|'; if (l1 < l2) l1++; else l1--; }
   
    graph[l1][c1] = '|'; //draw the first outcoming part of the link
}

