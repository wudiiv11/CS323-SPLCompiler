#ifndef __PARSER_H
#define __PARSER_H

#include "node.h"
#include "symbol_table.h"
#include <vector>

using namespace std;

class ExpItem {
    
public:
    Type* type;
    bool isLeftValue;
    ExpItem(Type* type, bool isLeftValue = false);
};

class Parser {

public:
    SymbolTable symbol_table;

    void       parse_tree            (Node*);
    void       parse_Program         (Node*);
    void       parse_ExtDefList      (Node*);
    void       parse_ExtDef          (Node*);
    void       parse_ExtDecList      (Node*, Type*);
    Type*      parse_Specifier       (Node*);
    Type*      parse_StructSpecifier (Node*);
    Field*     parse_VarDec          (Node*, Type*);
    void       parse_FunDec          (Node*, Type*);
    void       parse_VarList         (Node*, vector<Type*>*);
    Type*      parse_ParamDec        (Node*);
    void       parse_CompSt          (Node*, Type*);
    void       parse_StmtList        (Node*, Type*);
    void       parse_Stmt            (Node*, Type*);
    void       parse_DefList         (Node*, vector<Field*>*);
    void       parse_Def             (Node*, vector<Field*>*);
    void       parse_DecList         (Node*, Type*, vector<Field*>*);
    void       parse_Dec             (Node*, Type*, vector<Field*>*);
    ExpItem*   parse_Exp             (Node*);
    void       parse_Args            (Node*, vector<ExpItem*>*);
};


#endif