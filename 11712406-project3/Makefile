CXX=g++
FLEX=flex
BISON=bison


.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d syntax.y
splc: .lex .syntax
	@mkdir bin
	$(CXX) syntax.tab.c node.cpp parser.cpp symbol_table.cpp -lfl -ly -o bin/splc
	@chmod +x bin/splc
clean:
	@rm -rf bin/ lex.yy.c syntax.tab.*
.PHONY: splc