CXX=g++-10
FLEX=flex
BISON=bison


target=bin/splc
src=source/node.cpp source/translator.cpp source/store.cpp source/type.cpp syntax.tab.c
cflags=-std=c++11 -lfl -ly

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