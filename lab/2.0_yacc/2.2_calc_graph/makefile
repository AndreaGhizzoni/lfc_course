GCC=gcc
LEX=lex
YACC=yacc

# input file
LEXFILE=calc.l
YACCFILE=calc.y
INTERPRETER=graph.c
# out folder
BINFOLDER=bin
# out binary
BIN=calc

default: parser

parser: y.tab.o lex.yy.o
	$(RM) -rf $(BINFOLDER)
	@mkdir $(BINFOLDER)
	$(GCC) -o $(BINFOLDER)/$(BIN) y.tab.o lex.yy.o $(INTERPRETER) -ll -lm

y.tab.o: y.tab.h
	$(GCC) -c y.tab.c

y.tab.h: $(YACCFILE)
	$(YACC) -v -d $(YACCFILE)

lex.yy.o: lex.yy.c
	$(GCC) -c lex.yy.c

lex.yy.c: $(LEXFILE)
	$(LEX) $(LEXFILE)

# produce ONLY lex binary
lex:
	$(LEX) $(LEXFILE)
	$(GCC) -c lex.yy.c
	$(RM) -rf $(BINFOLDER)
	@mkdir $(BINFOLDER)
	$(GCC) -o $(BINFOLDER)/$(BIN).lex lex.yy.o -ll

# utility
clean:
	$(RM) -rf *.o lex.yy.c *.tab.* *.output $(BINFOLDER)

help:
	@echo "Usage:"
	@echo "make : default call on parser"
	@echo "make parser : to generate a executable parser bin/xxx"
	@echo "make lex : to generate a executable lex bin/xxx.lex"

