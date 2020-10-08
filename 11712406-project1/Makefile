CC=gcc
FLEX=flex
BISON=bison


.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d -v syntax.y
splc: .lex .syntax
	@mkdir bin
	$(CC) syntax.tab.c -lfl -ly -o bin/splc
	@chmod +x bin/splc
clean:
	@rm -rf bin/ syntax.tab.* syntax.output lex.yy.c
.PHONY: splc
