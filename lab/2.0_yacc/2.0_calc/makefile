GCC=gcc
LEX=lex
YACC=yacc

# input file
LEXFILE=calc.l
YACCFILE=calc.y
# out folder
BINFOLDER=bin
# out binary
BIN=calc

parser: y.tab.o lex.yy.o
	$(RM) -rf $(BINFOLDER)
	@mkdir $(BINFOLDER)
	$(GCC) -o $(BINFOLDER)/$(BIN) y.tab.o lex.yy.o -ll -lm

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
	@echo "make all : to generate a executable parser bin/xxx"
	@echo "make lex : to generate a executable lex bin/xxx.lex"

