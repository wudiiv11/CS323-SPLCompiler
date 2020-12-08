#ifndef __PARSER_H
#define __PARSER_H

#include "node.h"
#include "symbol_table.h"
#include <vector>
#include <cstdarg>
#include <iostream>

using namespace std;

class Record {

public:
    enum CATEGORY{
        R_INT, 
        R_ID, 
        R_ASSIGN, 
        R_PLUS, R_MINUS, R_MUL, R_DIV,
        R_LT, R_LE, R_GT, R_GE, R_NE, R_EQ,
        R_WHILE, R_AND, R_OR,
        R_GOTO, R_LABEL, R_RETURN,
    } category;
    vector<string> args;
    Record(CATEGORY c, vector<string> args);
    Record(CATEGORY c, int num, ...);

    string to_string() {
        switch (category) {
        case R_INT:
            return args[0] + " := #" + args[1];
        case R_ID:
            return args[0] + " := " + args[1];
        case R_ASSIGN:
            return args[0] + " := " + args[1];
        case R_PLUS:
            return args[0] + " := " + args[1] + " + " + args[2];
        case R_MINUS:
            return args[0] + " := " + args[1] + " - " + args[2];
        case R_MUL:
            return args[0] + " := " + args[1] + " * " + args[2];
        case R_DIV:
            return args[0] + " := " + args[1] + " / " + args[2];
        case R_LT:
            return "IF " + args[0] + " < " + args[1] + " GOTO " + args[2];
        case R_LE:
            return "IF " + args[0] + " <= " + args[1] + " GOTO " + args[2];
        case R_GT:
            return "IF " + args[0] + " > " + args[1] + " GOTO " + args[2];
        case R_GE:
            return "IF " + args[0] + " >= " + args[1] + " GOTO " + args[2];
        case R_NE:
            return "IF " + args[0] + " != " + args[1] + " GOTO " + args[2];
        case R_EQ:
            return "IF " + args[0] + " == " + args[1] + " GOTO " + args[2];
        case R_GOTO:
            return "GOTO " + args[0];
        case R_LABEL:
            return "LABEL " + args[0];
        case R_RETURN:
            return "RETURN " + args[0];
        default:
            break;
        }
    }
};

class ExpItem {
    
public:
    Type* type;
    bool isLeftValue;
    vector<Record*> tac;
    string id; // 如果是个变量, 则保存到id
    ExpItem();
    ExpItem(Type* type, string tac, bool isLeftValue = false);
    ExpItem(string id);
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
    ExpItem*   parse_Stmt            (Node*, Type*);
    void       parse_DefList         (Node*, vector<Field*>*);
    void       parse_Def             (Node*, vector<Field*>*);
    void       parse_DecList         (Node*, Type*, vector<Field*>*);
    void       parse_Dec             (Node*, Type*, vector<Field*>*);
    ExpItem*   parse_Exp             (Node*, string, string, string);
    void       parse_Args            (Node*, vector<ExpItem*>*);
};


#endif