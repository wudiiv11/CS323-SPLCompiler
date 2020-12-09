CXX=g++
FLEX=flex
BISON=bison


target=bin/splc
src=source/node.cpp source/translator.cpp source/store.cpp syntax.tab.c
cflags=-lfl -ly

.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d syntax.y
splc: .lex .syntax
	@mkdir bin
	$(CXX) $(src) $(cflags) -o $(target)
	@chmod +x bin/splc
clean:
	@rm -rf bin/ lex.yy.c syntax.tab.*
.PHONY: splc