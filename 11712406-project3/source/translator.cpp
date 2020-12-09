#include "../include/translator.h"

int Translator::place_cnt = 0;
int Translator::label_cnt = 0;

Translator::Translator() { codes; }

Record::Record(CATEGORY c, vector<string> args) {
    this->category = c;
    this->args = args;
}


Record::Record(CATEGORY c, int num, ...) {
    this->category = c;

    args;
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
    default:
        break;
    }
}


string Translator::new_place() {
    return string("_v" + to_string(place_cnt++));
}


string Translator::new_label() {
    return string("_lb" + to_string(label_cnt++));
}


Expr::Expr() {}
Expr::Expr(string id) : id(id) {}


void Translator::translate_tree(Node* n) {
    translate_Program(n);
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
    translate_Specifier(n->children[0]);
    if (n->children[1]->name == "ExtDecList")
        translate_ExtDecList(n->children[1]);
    if (n->children[1]->name == "FunDec") {
        translate_FunDec(n->children[1]);
        translate_CompSt(n->children[2]);
    }
}


void Translator::translate_ExtDecList(Node* n) {
    translate_VarDec(n->children[0]);
    if (n->children.size() > 1)
        translate_ExtDecList(n->children[2]);
}


void Translator::translate_Specifier(Node* n) {
    // nothing to translate
}


void Translator::translate_StructSpecifier(Node* n) {
    // todo
    // 这里应该返回一个结构体的信息
}


void Translator::translate_VarDec(Node* n) {
    // nothing to translate
}


void Translator::translate_FunDec(Node* n) {
    // todo
}


void Translator::translate_VarList(Node* n) {
    // todo
}


void Translator::translate_ParamDec(Node* n) {
    translate_Specifier(n->children[0]);
    translate_VarDec(n->children[1]);
}


void Translator::translate_CompSt(Node* n) {
    translate_DefList(n->children[1]);
    translate_StmtList(n->children[2]);
}


void Translator::translate_StmtList(Node* n) {
    if (n->children.empty())
        return;
    string s = n->children[0]->name;
    if (s == "Exp") {
        // 不应该出现这种情况
        translate_Exp(n->children[0], "null");
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


Expr* Translator::translate_Exp(Node* n, string place) {
    Expr* expr = new Expr();
    // todo 考虑 read / write

    string arg1 = n->children[0]->name;
    if (arg1 == "Exp") {
        string arg2 = n->children[1]->name;
        if (arg2 == "ASSIGN") {
            string tp1 = new_place();
            string tp2 = new_place();
            Expr* e = translate_Exp(n->children[0], tp1);
            translate_Exp(n->children[2], tp2);
            codes.push_back(Record(Record::R_ASSIGN, 2, e->id, tp2));
            codes.push_back(Record(Record::R_ASSIGN, 2, place, e->id));
        } else if (arg2 == "AND" || arg2 == "OR" || arg2 == "LT" || arg2 == "LE" || arg2 == "GT" || arg2 == "GE" || arg2 == "NE" || arg2 == "EQ") {
            string lb_1 = new_label();
            string lb_2 = new_label();
            codes.push_back(Record(Record::R_ASSIGN, 1, "#0"));
            translate_cond_Exp(n, lb_1, lb_2);
            codes.push_back(Record(Record::R_LABEL, 1, lb_1));
            codes.push_back(Record(Record::R_ASSIGN, 1, "#1"));
            codes.push_back(Record(Record::R_LABEL, 1, lb_2));
        } else if (arg2 == "PLUS" || arg2 == "MINUS" || arg2 == "MUL" || arg2 == "DIV") {
            string tp1 = new_place();
            string tp2 = new_place();
            translate_Exp(n->children[0], tp1);
            translate_Exp(n->children[2], tp2);
            if (arg2 == "PLUS") codes.push_back(Record(Record::R_PLUS, 2, tp1, tp2));
            else if (arg2 == "MINUS") codes.push_back(Record(Record::R_MINUS, 2, tp1, tp2));
            else if (arg2 == "MUL") codes.push_back(Record(Record::R_MUL, 2, tp1, tp2));
            else if (arg2 == "DIV") codes.push_back(Record(Record::R_DIV, 2, tp1, tp2));
        }
    } else if (arg1 == "LP") {
        
    } else if (arg1 == "MINUS") {
        string tp = new_place();
        translate_Exp(n->children[1], tp);
        codes.push_back(Record(Record::R_MINUS, 3, place, "#0", tp));
    } else if (arg1 == "NOT") {
        string lb_1 = new_label();
        string lb_2 = new_label();
        codes.push_back(Record(Record::R_ASSIGN, 1, "#0"));
        translate_cond_Exp(n, lb_2, lb_1);
        codes.push_back(Record(Record::R_LABEL, 1, lb_1));
        codes.push_back(Record(Record::R_ASSIGN, 1, "#1"));
        codes.push_back(Record(Record::R_LABEL, 1, lb_2));
    } else if (arg1 == "ID") {
        codes.push_back(Record(Record::R_ASSIGN, 1, n->children[0]->text));
        expr->id = n->children[0]->text;
    } else if (arg1 == "INT") {
        codes.push_back(Record(Record::R_INT, 1, n->children[0]->text));
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
