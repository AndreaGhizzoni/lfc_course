LEX = lex
YACC = yacc -d
CC = gcc -std=c99

interpreter: y.tab.o lex.yy.o functions.o interpreter.o
	@rm -f bin/
	@mkdir bin
	$(CC) -o bin/$@ y.tab.o lex.yy.o  functions.o interpreter.o 

lex.yy.o: lex.yy.c y.tab.h

y.tab.c y.tab.h: parser.y
	$(YACC) -v parser.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

functions.o: functions.c

interpreter.o: interpreter.c

clean:
	$(RM) -rf bin *.o lex.yy.c *.tab.*  interpreter *.output
