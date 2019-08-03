GCC=gcc
LEX=lex
OUTFOLDER=bin
LEXFILE=hrefintoancor.l
OUT=hrefintoancor

$(OUT): lex.yy.o
	$(RM) -rf $(OUTFOLDER)
	@mkdir $(OUTFOLDER)
	$(GCC) -o $(OUTFOLDER)/$(OUT) lex.yy.o -ll

lex.yy.o: lex.yy.c
	$(GCC) -c lex.yy.c

lex.yy.c: $(LEXFILE)
	$(LEX) $(LEXFILE)

clean :
	$(RM) -rf lex.yy.* $(OUTFOLDER)

