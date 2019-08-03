GCC=gcc
LEX=lex
LEXFILE=wordcounter.l
OUTFOLDER=bin
OUT=wordcounter

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

