#ifndef __TRANSLATOR_H
#define __TRANSLATOR_H

#include "node.h"
#include "store.h"
#include "type.h"
#include "optimizer.h"

#include <vector>

using namespace std;

class Translator {
private:
    static int place_cnt;
    static int field_cnt;
    static int label_cnt;

    Store store;
    Optimizer optimizer;
    vector<Record> codes;

public:
    string     new_place();
    string     new_field();
    string     new_label();

    vector<Record>* get_codes();

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
    Field*     translate_VarDec          (Node*, Type*, int);
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
    Expr*      translate_Exp             (Node*, string&);
    void       translate_Args            (Node*, vector<string>*);
    void       translate_cond_Exp        (Node*, string, string);
};

#endif