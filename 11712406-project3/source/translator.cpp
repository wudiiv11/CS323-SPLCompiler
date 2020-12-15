#include "../include/translator.h"

#include <iostream>
#include <algorithm>

int Translator::place_cnt = 0;
int Translator::field_cnt = 0;
int Translator::label_cnt = 0;

Translator::Translator() {}


string Translator::new_place() {
    return string("t" + to_string(++place_cnt));
}


string Translator::new_field() {
    return string("v" + to_string(++field_cnt));
}


string Translator::new_label() {
    return string("label" + to_string(++label_cnt));
}


void Translator::translate_tree(Node* n) {
    translate_Program(n);
    for (auto i : codes)
        cout << i.to_string() << endl;
}


void Translator::translate_Program(Node* n) {
    translate_ExtDefList(n->children[0]);
}


void Translator::translate_ExtDefList(Node* n) {
    if (n->children.size() > 0)
        translate_ExtDef(n->children[0]);
    if (n->children.size() > 1)
        translate_ExtDefList(n->children[1]);
}


void Translator::translate_ExtDef(Node* n) {
    Type* t = translate_Specifier(n->children[0]);
    if (n->children[1]->name == "ExtDecList")
        translate_ExtDecList(n->children[1], t);
    if (n->children[1]->name == "FunDec") {

        Function* f = translate_FunDec(n->children[1], t);
        Type* t = new Type(f);
        store.insert(f->name, f->name, t, false);
        translate_CompSt(n->children[2]);
    }
}


void Translator::translate_ExtDecList(Node* n, Type* t) {
    translate_VarDec(n->children[0], t, false);
    if (n->children.size() > 1)
        translate_ExtDecList(n->children[2], t);
}


Type* Translator::translate_Specifier(Node* n) {
    Type* t = new Type();
    Node* child = n->children[0];
    if (child->name == "TYPE") {
        t->primitive = new string(child->text);
        t->category = Type::T_PRIMITIVE;
    } else if (child->name == "StructSpecifier") {
        t->structure = translate_StructSpecifier(child);
        t->category = Type::T_STRUCTURE;
    }

    return t;
}


Struct* Translator::translate_StructSpecifier(Node* n) {

    string name = n->children[1]->text;
    Struct* ret;
    if (n->children.size() == 5) {
        vector<Field*>* fields = new vector<Field*>();

        store.add_scope();
        translate_DefList(n->children[3], fields);
        declare_size(fields);
        store.sub_scope();

        ret = new Struct(name, fields);
        store.insert(name, name, new Type(ret), false);
    } else {
        Item* item = store.lookup(name);
        ret = store.lookup(name, Type::T_STRUCTURE)->t->structure;
    }

    return ret;
}


Field* Translator::translate_VarDec(Node* n, Type* t, int is_pointer) {
    if (n->children[0]->name == "ID") {
        string name = n->children[0]->text;
        string alias = new_field();
        Field* f = new Field(name, t);
        store.insert(name, alias, t, is_pointer);
        return f;
    } else if (n->children[0]->name == "VarDec") {
        Array* arr = new Array(t, atoi(n->children[2]->text.c_str()));
        Type* type = new Type(arr);
        return translate_VarDec(n->children[0], type, is_pointer);        
    }
}


Function* Translator::translate_FunDec(Node* n, Type* t) {
    Function* f = new Function();
    f->name = n->children[0]->text;
    f->ret = t;
    codes.push_back(Record(Record::R_FUNCTION, 1, f->name));
    vector<Field*>* args = new vector<Field*>();
    if (n->children[2]->name == "VarList") 
        translate_VarList(n->children[2], args);
    for (auto i : *args) {
        codes.push_back(Record(Record::R_PARAM, 1, store.lookup(i->name)->alias));
    }
    f->args = args;
    return f;
}


void Translator::translate_VarList(Node* n, vector<Field*>* fields) {
    Field* f = translate_ParamDec(n->children[0]);
    fields->push_back(f);
    if (n->children.size() > 1)
        translate_VarList(n->children[2], fields);
}


Field* Translator::translate_ParamDec(Node* n) {
    Type* t = translate_Specifier(n->children[0]);
    Field* f = translate_VarDec(n->children[1], t, true);
    return f;
}


void Translator::declare_size(vector<Field*>* fields) {
    for (auto i : *fields) {
        Item* item = store.lookup(i->name);
        if (item->t->size() > 4)
            codes.push_back(Record(Record::R_DEC, 2, item->alias, to_string(item->t->size())));
    }
}


void Translator::translate_CompSt(Node* n) {
    store.add_scope();

    vector<Field*>* fields = new vector<Field*>();

    if (n->children.size() == 3) {
        if (n->children[1]->name == "DefList") {
            translate_DefList(n->children[1], fields);
            declare_size(fields);
        } else
            translate_StmtList(n->children[1]);
    } else if (n->children.size() == 4) {
        translate_DefList(n->children[1], fields);
        declare_size(fields);
        translate_StmtList(n->children[2]);
    }

    store.sub_scope();
}


void Translator::translate_StmtList(Node* n) {
    if (n->children.size() > 0)
        translate_Stmt(n->children[0]);
    if (n->children.size() > 1)
        translate_StmtList(n->children[1]);
}


void Translator::translate_Stmt(Node* n) {
    string s = n->children[0]->name;
    if (s == "Exp") {
        string tp = new_place();
        translate_Exp(n->children[0], tp);
    } else if (s == "CompSt") {
        translate_CompSt(n->children[0]);
    } else if (s == "RETURN") {
        string tp = new_place();
        translate_Exp(n->children[1], tp);
        codes.push_back(Record(Record::R_RETURN, 1, tp));
    } else if (s == "IF") {
        if (n->children.size() > 5) {
            string lb_1 = new_label();
            string lb_2 = new_label();
            string lb_3 = new_label();
            translate_cond_Exp(n->children[2], lb_1, lb_2);
            codes.push_back(Record(Record::R_LABEL, 1, lb_1));
            translate_Stmt(n->children[4]);
            codes.push_back(Record(Record::R_GOTO, 1, lb_3));
            codes.push_back(Record(Record::R_LABEL, 1, lb_2));
            translate_Stmt(n->children[6]);
            codes.push_back(Record(Record::R_LABEL, 1, lb_3));
        } else {
            string lb_1 = new_label();
            string lb_2 = new_label();
            translate_cond_Exp(n->children[2], lb_1, lb_2);
            codes.push_back(Record(Record::R_LABEL, 1, lb_1));
            translate_Stmt(n->children[4]);
            codes.push_back(Record(Record::R_LABEL, 1, lb_2));
        }
    } else if (s == "WHILE") {
        string lb_1 = new_label();
        string lb_2 = new_label();
        string lb_3 = new_label();
        codes.push_back(Record(Record::R_LABEL, 1, lb_1));
        translate_cond_Exp(n->children[2], lb_2, lb_3);
        codes.push_back(Record(Record::R_LABEL, 1, lb_2));
        translate_Stmt(n->children[4]);
        codes.push_back(Record(Record::R_GOTO, 1, lb_1)); 
        codes.push_back(Record(Record::R_LABEL, 1, lb_3));
    }
}


void Translator::translate_Dec(Node* n, Type* t, vector<Field*>* fields) {
    Field* f = translate_VarDec(n->children[0], t, false);
    fields->push_back(f);
    if (n->children.size() > 1) {
        string tp = new_place();
        translate_Exp(n->children[2], tp);
        Item* item = store.lookup(f->name);
        codes.push_back(Record(Record::R_ASSIGN, 2, item->alias, tp));
    }
}


void Translator::translate_Def(Node* n, vector<Field*>* fields) {
    Type* t = translate_Specifier(n->children[0]);
    translate_DecList(n->children[1], t, fields);
}


void Translator::translate_DecList(Node* n, Type* t, vector<Field*>* fields) {
    translate_Dec(n->children[0], t, fields);
    if (n->children.size() > 1)
        translate_DecList(n->children[2], t, fields);
}


void Translator::translate_DefList(Node* n, vector<Field*>* fields) {
    if (n->children[0]->name == "Def")
        translate_Def(n->children[0], fields);
    if (n->children.size() > 1)
        translate_DefList(n->children[1], fields);
}


Expr* Translator::translate_Exp(Node* n, string& place) {
    Expr* expr = new Expr();

    string arg1 = n->children[0]->name;
    if (arg1 == "Exp") {
        string arg2 = n->children[1]->name;
        if (arg2 == "ASSIGN") {
            string t0 = new_place();
            Expr* e = translate_Exp(n->children[0], t0);
            string t1 = new_place();
            translate_Exp(n->children[2], t1);
            codes.push_back(Record(Record::R_ASSIGN, 2, t0, t1));
            place = t0;
        } else if (arg2 == "AND" || arg2 == "OR" || arg2 == "LT" || arg2 == "LE" || arg2 == "GT" || arg2 == "GE" || arg2 == "NE" || arg2 == "EQ") {
            string lb_1 = new_label();
            string lb_2 = new_label();
            codes.push_back(Record(Record::R_ASSIGN, 2, place, string("#0")));
            translate_cond_Exp(n, lb_1, lb_2);
            codes.push_back(Record(Record::R_LABEL, 1, lb_1));
            codes.push_back(Record(Record::R_ASSIGN, 2, place, string("#1")));
            codes.push_back(Record(Record::R_LABEL, 1, lb_2));
        } else if (arg2 == "PLUS" || arg2 == "MINUS" || arg2 == "MUL" || arg2 == "DIV") {
            string tp1 = new_place();
            string tp2 = new_place();
            Expr* e1 = translate_Exp(n->children[0], tp1);
            Expr* e2 = translate_Exp(n->children[2], tp2);
            if (arg2 == "PLUS") codes.push_back(Record(Record::R_PLUS, 3, place, tp1, tp2));
            else if (arg2 == "MINUS") codes.push_back(Record(Record::R_MINUS, 3, place, tp1, tp2));
            else if (arg2 == "MUL") codes.push_back(Record(Record::R_MUL, 3, place, tp1, tp2));
            else if (arg2 == "DIV") codes.push_back(Record(Record::R_DIV, 3, place, tp1, tp2));
            // 这里type的判断需要加一个函数识别自动向上转型, 暂时先默认为第一个type
            expr->t = e1->t;
        } else if (arg2 == "DOT") {
            string tp = new_place();
            Expr* e = translate_Exp(n->children[0], tp);
            int offset = e->t->structure->offset_of(n->children[2]->text);
            string addr = new_place();
            codes.push_back(Record(Record::R_OFFSET, 3, addr, e->addr, to_string(offset)));
            place = "*" + addr;
        } else if (arg2 == "LB") {
            string tp1 = new_place();
            string tp2 = new_place();
            string tp3 = new_place();
            string tp4 = new_place();
            Expr* e1 = translate_Exp(n->children[0], tp1);
            Expr* e2 = translate_Exp(n->children[2], tp2);
            codes.push_back(Record(Record::R_MUL, 3, tp3, "#" + to_string(e1->t->array->base->size()), tp2));
            codes.push_back(Record(Record::R_PLUS, 3, tp4, e1->addr, tp3));
            
            place = "*" + tp4;
            expr->t = e1->t->array->base;
            expr->addr = tp4;
        }
    } else if (arg1 == "LP") {
        translate_Exp(n->children[1], place);
    } else if (arg1 == "MINUS") {
        string tp = new_place();
        Expr* e = translate_Exp(n->children[1], tp);
        codes.push_back(Record(Record::R_MINUS, 3, place, string("#0"), tp));
        expr = e;
    } else if (arg1 == "NOT") {
        string lb_1 = new_label();
        string lb_2 = new_label();
        codes.push_back(Record(Record::R_ASSIGN, 1, "#0"));
        translate_cond_Exp(n, lb_2, lb_1);
        codes.push_back(Record(Record::R_LABEL, 1, lb_1));
        codes.push_back(Record(Record::R_ASSIGN, 1, "#1"));
        codes.push_back(Record(Record::R_LABEL, 1, lb_2));
    } else if (arg1 == "ID") {
        // 对 read/write 函数进行特判
        if (n->children.size() == 1) {
            string var = n->children[0]->text;
            Item* item = store.lookup(var);
            place = item->alias;
            expr->t = item->t;
            expr->is_pointer = item->is_pointer;
            if (item->is_pointer)
                expr->addr = item->alias;
            else
                expr->addr = "&" + item->alias;
        } else if (n->children[0]->text == "read") {
            codes.push_back(Record(Record::R_READ, 1, place));
        } else if (n->children[0]->text == "write") {
            string tp = new_place();
            translate_Exp(n->children[2]->children[0], tp);
            codes.push_back(Record(Record::R_WRITE, 1, tp));
        } else {
            Item* item = store.lookup(n->children[0]->text);
            if (n->children[2]->name == "Args") {
                vector<string>* args = new vector<string>();
                translate_Args(n->children[2], args);
                reverse(args->begin(), args->end());
                for (auto i : *args) {
                    codes.push_back(Record(Record::R_ARG, 1, i));
                }
            }
            codes.push_back(Record(Record::R_CALL, 2, place, item->alias));
        }
    } else if (arg1 == "INT") {
        place = "#" + n->children[0]->text;
        expr->t = new Type(new string("int"));
    }

    return expr;
}


void Translator::translate_Args(Node* n, vector<string>* args) {
    string tp = new_place();
    Expr* e = translate_Exp(n->children[0], tp);
    if (e->t->category != Type::T_PRIMITIVE && !e->is_pointer)
        tp = "&" + tp;
    args->push_back(tp);
    if (n->children.size() > 1)
        translate_Args(n->children[2], args);
}


void Translator::translate_cond_Exp(Node* n, string lb_t, string lb_f) {
    string op = n->children[1]->name;
    if (op == "AND" || op == "OR") {
        string lb = new_label();
        if (op == "AND")
            translate_cond_Exp(n->children[0], lb, lb_f);
        else 
            translate_cond_Exp(n->children[0], lb_t, lb);
        codes.push_back(Record(Record::R_LABEL, 1, lb));
        translate_cond_Exp(n->children[2], lb_t, lb_f);
    } else if (op == "LT" || op == "LE" || op == "GT" || op == "GE" || op == "NE" || op == "EQ") {
        string tp1 = new_place();
        string tp2 = new_place();
        translate_Exp(n->children[0], tp1);
        translate_Exp(n->children[2], tp2);

        if (op == "LT") codes.push_back(Record(Record::R_LT, 3, tp1, tp2, lb_t));
        if (op == "LE") codes.push_back(Record(Record::R_LE, 3, tp1, tp2, lb_t));
        if (op == "GT") codes.push_back(Record(Record::R_GT, 3, tp1, tp2, lb_t));
        if (op == "GE") codes.push_back(Record(Record::R_GE, 3, tp1, tp2, lb_t));
        if (op == "NE") codes.push_back(Record(Record::R_NE, 3, tp1, tp2, lb_t));
        if (op == "EQ") codes.push_back(Record(Record::R_EQ, 3, tp1, tp2, lb_t));
        
        codes.push_back(Record(Record::R_GOTO, 1, lb_f));
    }
}

