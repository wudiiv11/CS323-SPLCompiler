#include "../include/translator.h"

#include <iostream>

int Translator::place_cnt = 0;
int Translator::field_cnt = 0;
int Translator::label_cnt = 0;

Translator::Translator() {}

Record::Record(CATEGORY c, vector<string> args) {
    this->category = c;
    this->args = args;
}


Record::Record(CATEGORY c, int num, ...) {
    this->category = c;

    va_list list;
    va_start(list, num);

    for (int i = 0; i < num; ++i) {
        string* p = va_arg(list, string*);
        args.push_back(*p);
    }

    va_end(list);
}


string Record::to_string() {
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
    case R_FUNCTION:
        return "FUNCTION " + args[0] + " :";
    case R_PARAM:
        return "PARAM " + args[0];
    case R_READ:
        return "READ " + args[0];
    case R_WRITE:
        return "WRITE " + args[0];
    case R_CALL:
    default:
        return "error";
    }
}


string Translator::new_place() {
    return string("t" + to_string(place_cnt++));
}


string Translator::new_field() {
    return string("v" + to_string(field_cnt++));
}


string Translator::new_label() {
    return string("label" + to_string(label_cnt++));
}


Expr::Expr() {}
Expr::Expr(string id) : id(id) {}


void Translator::init_read_func() {
    Function* f = new Function();
    f->args = new vector<Field*>();
    f->ret = new Type("int");
    f->name = "read";

    store.insert("read", "READ", new Type(f));
    Item* item = store.lookup("read");
}


void Translator::init_write_func() {
    Function* f = new Function();
    f->args = new vector<Field*>();
    f->args->push_back(new Field("arg", new Type("int")));
    f->ret = new Type("int");
    f->name = "write";

    store.insert("write", "WRITE", new Type(f));
}


void Translator::init_sys_call() {
    init_read_func();
    init_write_func();
}


void Translator::translate_tree(Node* n) {
    init_sys_call();

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
        store.insert(f->name, "none", t);

        translate_CompSt(n->children[2]);
    }
}


void Translator::translate_ExtDecList(Node* n, Type* t) {
    translate_VarDec(n->children[0], t);
    if (n->children.size() > 1)
        translate_ExtDecList(n->children[2], t);
}


Type* Translator::translate_Specifier(Node* n) {
    Type* t = new Type();
    Node* child = n->children[0];
    if (n->name == "TYPE") {
        t->primitive = child->text;
        t->category = Type::T_PRIMITIVE;
    } else if (n->name == "StructSpecifier") {
        t->structure = translate_StructSpecifier(child);
        t->category = Type::T_STRUCTURE;
    }
    return t;
}


Struct* Translator::translate_StructSpecifier(Node* n) {

    store.add_scope();

    string name = n->children[1]->text;
    vector<Field*>* fields = new vector<Field*>();
    if (n->children.size() == 5)
        translate_DefList(n->children[3], fields);

    Struct* ret = new Struct(name, fields);

    store.sub_scope();

    return ret;
}


Field* Translator::translate_VarDec(Node* n, Type* t) {
    if (n->children[0]->name == "ID") {
        string name = n->children[0]->text;
        string alias = new_field();
        Field* f = new Field(name, t);
        store.insert(name, alias, t);
        return f;
    } else if (n->children[0]->name == "VarDec") {
        Array* arr = new Array(t, atoi(n->children[2]->text.c_str()));
        Type* type = new Type(arr);
        return translate_VarDec(n->children[0], type);        
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
    f->args = args;
    return f;
}


void Translator::translate_VarList(Node* n, vector<Field*>* fields) {
    Field* f = translate_ParamDec(n->children[0]);
    fields->push_back(f);
}


Field* Translator::translate_ParamDec(Node* n) {
    Type* t = translate_Specifier(n->children[0]);
    Field* f = translate_VarDec(n->children[1], t);
    return f;
}


void Translator::translate_CompSt(Node* n) {
    store.add_scope();

    translate_DefList(n->children[1], new vector<Field*>());
    translate_StmtList(n->children[2]);

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
        string lb_1 = new_label();
        string lb_2 = new_label();
        string lb_3 = new_label();
        translate_cond_Exp(n->children[2], lb_1, lb_2);
        codes.push_back(Record(Record::R_LABEL, 1, lb_1));
        translate_Stmt(n->children[4]);
        codes.push_back(Record(Record::R_LABEL, 1, lb_2));
        if (n->children.size() > 5) {
            translate_Stmt(n->children[6]);
            codes.push_back(Record(Record::R_LABEL, 1, lb_3));
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


void Translator::translate_Dec(Node* n, Type* t) {
    translate_VarDec(n->children[0], t);
    if (n->children.size() > 1) {
        string tp = new_place();
        translate_Exp(n->children[2], tp);
    }
}


void Translator::translate_Def(Node* n) {
    Type* t = translate_Specifier(n->children[0]);
    translate_DecList(n->children[1], t);
}


void Translator::translate_DecList(Node* n, Type* t) {
    translate_Dec(n->children[0], t);
    if (n->children.size() > 1)
        translate_DecList(n->children[2], t);
}


void Translator::translate_DefList(Node* n, vector<Field*>* fields) {
    if (n->children[0]->name == "Def")
        translate_Def(n->children[0]);
    if (n->children.size() > 1)
        translate_DefList(n->children[1], fields);
}


Expr* Translator::translate_Exp(Node* n, string place) {
    Expr* expr = new Expr();
    // todo 考虑 read / write

    // cout << n->children[0]->name << endl;

    string arg1 = n->children[0]->name;
    if (arg1 == "Exp") {
        string arg2 = n->children[1]->name;
        if (arg2 == "ASSIGN") {
            string var = n->children[0]->children[0]->text;
            Item* item = store.lookup(var);
            string var_alias = item->alias;
            string tp = new_place();
            translate_Exp(n->children[2], tp);
            codes.push_back(Record(Record::R_ASSIGN, 2, var_alias, tp));
            codes.push_back(Record(Record::R_ASSIGN, 2, place, var_alias));
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
            translate_Exp(n->children[0], tp1);
            translate_Exp(n->children[2], tp2);
            if (arg2 == "PLUS") codes.push_back(Record(Record::R_PLUS, 3, place, tp1, tp2));
            else if (arg2 == "MINUS") codes.push_back(Record(Record::R_MINUS, 3, place, tp1, tp2));
            else if (arg2 == "MUL") codes.push_back(Record(Record::R_MUL, 3, place, tp1, tp2));
            else if (arg2 == "DIV") codes.push_back(Record(Record::R_DIV, 3, place, tp1, tp2));
        }
    } else if (arg1 == "LP") {
        translate_Exp(n->children[1], place);
    } else if (arg1 == "MINUS") {
        string tp = new_place();
        translate_Exp(n->children[1], tp);
        codes.push_back(Record(Record::R_MINUS, 3, place, string("#0"), tp));
    } else if (arg1 == "NOT") {
        string lb_1 = new_label();
        string lb_2 = new_label();
        codes.push_back(Record(Record::R_ASSIGN, 1, "#0"));
        translate_cond_Exp(n, lb_2, lb_1);
        codes.push_back(Record(Record::R_LABEL, 1, lb_1));
        codes.push_back(Record(Record::R_ASSIGN, 1, "#1"));
        codes.push_back(Record(Record::R_LABEL, 1, lb_2));
    } else if (arg1 == "ID") {
        if (n->children.size() == 1) {
            string var = n->children[0]->text;
            Item* item = store.lookup(var);
            codes.push_back(Record(Record::R_ASSIGN, 2, place, item->alias));                
            expr->id = n->children[0]->text;
        } else if (n->children[0]->text == "read") {
            codes.push_back(Record(Record::R_READ, 1, place));
        } else if (n->children[0]->text == "write") {
            string tp = new_place();
            translate_Exp(n->children[2]->children[0], tp);
            codes.push_back(Record(Record::R_WRITE, 1, place));
        } else if (n->children[2]->name == "Args") {
        
        } else {
            // Item* item = store.lookup(n->children[0]->text);
            // codes.push_back(Record(Record::R_CALL, 2, item->alias, place));
        }
    } else if (arg1 == "INT") {
        codes.push_back(Record(Record::R_INT, 2, place, n->children[0]->text));
    }

    return expr;
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

        // 这里可以包装成一个函数
        if (op == "LT") codes.push_back(Record(Record::R_LT, 3, tp1, tp2, lb_t));
        if (op == "LE") codes.push_back(Record(Record::R_LE, 3, tp1, tp2, lb_t));
        if (op == "GT") codes.push_back(Record(Record::R_GT, 3, tp1, tp2, lb_t));
        if (op == "GE") codes.push_back(Record(Record::R_GE, 3, tp1, tp2, lb_t));
        if (op == "NE") codes.push_back(Record(Record::R_NE, 3, tp1, tp2, lb_t));
        if (op == "EQ") codes.push_back(Record(Record::R_EQ, 3, tp1, tp2, lb_t));
        
        codes.push_back(Record(Record::R_GOTO, 1, lb_f));
    }
}

