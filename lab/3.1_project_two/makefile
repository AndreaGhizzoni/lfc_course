#157507
LEX = lex
YACC = yacc -d
CC = gcc -std=c99

default: interpreter

interpreter: y.tab.o lex.yy.o functions.o interpreter.o
	$(CC) -o $@ y.tab.o lex.yy.o functions.o interpreter.o 

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: parser.y
	$(YACC) -v parser.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

functions.o: functions.c

interpreter.o: interpreter.c

clean:
	$(RM) -rf *.o lex.yy.c *.tab.* interpreter *.output
