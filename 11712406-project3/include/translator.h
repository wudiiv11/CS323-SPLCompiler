#ifndef __TRANSLATOR_H
#define __TRANSLATOR_H

#include "node.h"
#include "exp_item.h"
#include "record.h"

#include <vector>
#include <cstdarg>

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

    string to_string();
};

class Expr {

public:
    Expr();
    Expr(string id);

    string id;
};

class Translator {
private:
    static int place_cnt;
    static int label_cnt;

    vector<Record> codes;

public:

    Translator();

    string     new_place();
    string     new_label();

    void       translate_tree            (Node*);
    void       translate_Program         (Node*);
    void       translate_ExtDefList      (Node*);
    void       translate_ExtDef          (Node*);
    void       translate_ExtDecList      (Node*);
    void       translate_Specifier       (Node*);
    void       translate_StructSpecifier (Node*);
    void       translate_VarDec          (Node*);
    void       translate_FunDec          (Node*);
    void       translate_VarList         (Node*);
    void       translate_ParamDec        (Node*);
    void       translate_CompSt          (Node*);
    void       translate_StmtList        (Node*);
    void       translate_Stmt            (Node*);
    void       translate_DefList         (Node*);
    void       translate_Def             (Node*);
    void       translate_DecList         (Node*);
    void       translate_Dec             (Node*);
    Expr*      translate_Exp             (Node*, string);
    void       translate_Args            (Node*);
    void       translate_cond_Exp        (Node*, string, string);
};

#endif