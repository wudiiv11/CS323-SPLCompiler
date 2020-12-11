#ifndef __TRANSLATOR_H
#define __TRANSLATOR_H

#include "node.h"
#include "store.h"
#include "type.h"

#include <vector>
#include <cstdarg>

using namespace std;

class Record {

public:
    enum CATEGORY {
        R_INT, R_ID, R_FUNCTION,
        R_ASSIGN, R_OFFSET,
        R_PLUS, R_MINUS, R_MUL, R_DIV,
        R_LT, R_LE, R_GT, R_GE, R_NE, R_EQ,
        R_WHILE, R_AND, R_OR,
        R_GOTO, R_LABEL, R_READ, R_WRITE,
        R_RETURN, R_CALL, R_PARAM, R_ARG, R_DEC,
    } category;
    vector<string> args;
    Record(CATEGORY c, vector<string> args);
    Record(CATEGORY c, int num, ...);

    string to_string();
};

class Expr {

public:
    string addr;
    Type* t;

    Expr();
    Expr(string id);
};

class Translator {
private:
    static int place_cnt;
    static int field_cnt;
    static int label_cnt;

    Store store;
    vector<Record> codes;

public:
    string     new_place();
    string     new_field();
    string     new_label();

    Translator();

    /* 在转换成三地址码时配合translate_DefList声明变量大小 */
    void       declare_size              (vector<Field*>*);

    void       translate_tree            (Node*);
    void       translate_Program         (Node*);
    void       translate_ExtDefList      (Node*);
    void       translate_ExtDef          (Node*);
    void       translate_ExtDecList      (Node*, Type*);
    Type*      translate_Specifier       (Node*);
    Struct*    translate_StructSpecifier (Node*);
    Field*     translate_VarDec          (Node*, Type*);
    Function*  translate_FunDec          (Node*, Type*);
    void       translate_VarList         (Node*, vector<Field*>*);
    Field*     translate_ParamDec        (Node*);
    void       translate_CompSt          (Node*);
    void       translate_StmtList        (Node*);
    void       translate_Stmt            (Node*);
    void       translate_DefList         (Node*, vector<Field*>*);
    void       translate_Def             (Node*, vector<Field*>*);
    void       translate_DecList         (Node*, Type*, vector<Field*>*);
    void       translate_Dec             (Node*, Type*, vector<Field*>*);
    Expr*      translate_Exp             (Node*, string);
    void       translate_Args            (Node*, vector<string>*);
    void       translate_cond_Exp        (Node*, string, string);
};

#endif