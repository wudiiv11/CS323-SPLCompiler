#include "parser.h"
#include <vector>
#include <iostream>

using namespace std;

unsigned long long v_cnt = 0;   /* 自动生成变量的数量 */
unsigned long long lb_cnt = 0;  /* 自动生成标签的数量*/

string new_place() {
    return string("_v" + to_string(v_cnt++));
}

string new_label() {
    return string("_lb" + to_string(lb_cnt++));
}

void Parser::parse_tree(Node* n) {
    parse_Program(n);
}

void Parser::parse_Program(Node* n) {
    parse_ExtDefList(n->children[0]);
}

void Parser::parse_ExtDefList(Node* n) {
    if (n->children.size() > 0)
        parse_ExtDef(n->children[0]);
    if (n->children.size() > 1)
        parse_ExtDefList(n->children[1]);
}

void Parser::parse_ExtDef(Node* n) {
    Type* t = parse_Specifier(n->children[0]);
    if (n->children[1]->name == "ExtDecList")
        parse_ExtDecList(n->children[1], t);
    if (n->children[1]->name == "FunDec") {
        parse_FunDec(n->children[1], t);
        parse_CompSt(n->children[2], t);
    }
}

void Parser::parse_ExtDecList(Node* n, Type* t) {
    parse_VarDec(n->children[0], t);
    if (n->children.size() > 1)
        parse_ExtDecList(n->children[2], t);
}

Type* Parser::parse_Specifier(Node* n) {
    Node* child = n->children[0];
    if (child->name == "TYPE")
        return new Type(child->text);
    if (child->name == "StructSpecifier")
        return parse_StructSpecifier(child);
    return NULL;
}

Type* Parser::parse_StructSpecifier(Node* n) {
    Type* t;
    if (n->children.size() == 2) {
        // 这里有坑
        t = new Type(n->children[1]->text, new vector<Field*>());
    } else if (n->children.size() == 5) {
        vector<Field*>* fields = new vector<Field*>();
        parse_DefList(n->children[3], fields);
        t = new Type(n->children[1]->name, fields);
    }
    return t;
}

Field* Parser::parse_VarDec(Node* n, Type* t) {
    if (n->children[0]->name == "ID") {
        return new Field(n->children[0]->text, t);
    } else if (n->children[0]->name == "VarDec") {
        Type* t2 = new Type(new Array(t, atoi(n->children[2]->text.c_str())));
        return parse_VarDec(n->children[0], t2);
    }
}

void Parser::parse_FunDec(Node* n, Type* t) {
    vector<Type*>* args = new vector<Type*>();
    if (n->children[2]->name == "VarList") {
        parse_VarList(n->children[2], args);
    }
}

void Parser::parse_VarList(Node* n, vector<Type*>* args) {
    args->push_back(parse_ParamDec(n->children[0]));
    if (n->children.size() > 1) 
        parse_VarList(n->children[2], args);
}

Type* Parser::parse_ParamDec(Node* n) {
    Type* t = parse_Specifier(n->children[0]);
    parse_VarDec(n->children[1], t);
    return t;
}

void Parser::parse_CompSt(Node* n, Type* t) {
    // 此处可能有坑
    parse_DefList(n->children[1], new vector<Field*>());
    parse_StmtList(n->children[2], t);
}

void Parser::parse_StmtList(Node* n, Type* t) {
    if (n->children[0]->name == "Stmt") {
        parse_Stmt(n->children[0], t);
        // parse_StmtList(n->children[1], t);
    }
}

ExpItem* Parser::parse_Stmt(Node* n, Type* t) {
    ExpItem* ret = new ExpItem();

    if (n->children[0]->name == "Exp") {
        string tp = new_place();
        // ExpItem* e = parse_Exp(n->children[0]);
        ret = parse_Exp(n->children[0], tp, "", "");
     } //else if (n->children[0]->name == "CompSt") {
    //     parse_CompSt(n->children[0], t);
    // } else if (n->children[0]->name == "RETURN") {
    //     string tp = new_place();
    //     ExpItem* e = parse_Exp(n->children[1], tp, "", "");
    //     ret->tac.insert(ret->tac.end(), e->tac.begin(), e->tac.end());
    //     ret->tac.push_back(new Record(Record::R_RETURN, 1, tp));
    // } else if (n->children[0]->name == "IF") {
    //     string lb_1 = new_label();
    //     string lb_2 = new_label();
    //     string lb_3 = new_label();
    //     ExpItem* e1 = parse_Exp(n->children[2], "", lb_1, lb_2);
    //     ExpItem* e2 = parse_Stmt(n->children[4], t);
    //     ret->tac.insert(ret->tac.end(), e1->tac.begin(), e1->tac.end());
    //     ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_1));
    //     ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());
    //     ret->tac.push_back(new Record(Record::R_GOTO, 1, lb_3));
    //     ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_2));
    //     if(n->children.size() > 6) {
    //         ExpItem* e3 = parse_Stmt(n->children[6], t);
    //         ret->tac.insert(ret->tac.end(), e3->tac.begin(), e3->tac.end());
    //         ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_3));
    //     }
    // } else if (n->children[0]->name == "WHILE") {
    //     string lb_1 = new_label();
    //     string lb_2 = new_label();
    //     string lb_3 = new_label();
    //     ExpItem* e1 = parse_Exp(n->children[2], "", lb_2, lb_3);
    //     ExpItem* e2 = parse_Stmt(n->children[4], t);
    //     ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_1));
    //     ret->tac.insert(ret->tac.end(), e1->tac.begin(), e1->tac.end());
    //     ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_2));
    //     ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());
    //     ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_1));
    // }

    for(auto i : ret->tac) {
        cout << (*i).to_string() << endl;
    }
    
    return ret;
}

void Parser::parse_DefList(Node* n, vector<Field*>* fields) {
    if (n->children[0]->name == "Def")
        parse_Def(n->children[0], fields);
    if (n->children.size() > 1)
        parse_DefList(n->children[1], fields);
}

void Parser::parse_Def(Node* n, vector<Field*>* fields) {
    Type* t = parse_Specifier(n->children[0]);
    parse_DecList(n->children[1], t, fields);
}

void Parser::parse_DecList(Node* n, Type* t, vector<Field*>* fields) {
    parse_Dec(n->children[0], t, fields);
    if (n->children.size() > 1)
        parse_DecList(n->children[2], t, fields);
}

void Parser::parse_Dec(Node* n, Type* t, vector<Field*>* fields) {
    parse_VarDec(n->children[0], t);
    if (n->children.size() > 1)
        parse_DecList(n->children[2], t, fields);
}

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

ExpItem::ExpItem() {
    isLeftValue = false;
    type = nullptr;
    tac;
}

ExpItem::ExpItem(string id) {
    this->id = id;
}

ExpItem* Parser::parse_Exp(Node* n, string place = "", string lb_t = "", string lb_f = "") {
    ExpItem* ret = new ExpItem();
    string op1 = n->children[0]->name;
    if (op1 == "Exp") {
        string op2 = n->children[1]->name;
        if (op2 == "ASSIGN") {
            string tp1 = new_place();
            string tp2 = new_place();
            ExpItem* e1 = parse_Exp(n->children[0], tp1);
            ExpItem* e2 = parse_Exp(n->children[2], tp2);
            ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());
            ret->tac.push_back(new Record(Record::R_ASSIGN, 2, e1->id, tp2));
            ret->tac.push_back(new Record(Record::R_ASSIGN, 2, place, e1->id));
            Record r1(Record::R_ASSIGN, 2, e1->id, tp2);
            Record r2(Record::R_ASSIGN, 2, place, e1->id);
        } else if (op2 == "AND") {
            string lb_1 = new_label();
            ExpItem* e1 = parse_Exp(n->children[0], "null", lb_1, lb_f);
            ExpItem* e2 = parse_Exp(n->children[2], "null", lb_t, lb_f);
            ret->tac.insert(ret->tac.end(), e1->tac.begin(), e1->tac.end());
            ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_1));
            ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());
        } else if (op2 == "OR") {
            string lb_1 = new_label();
            ExpItem* e1 = parse_Exp(n->children[0], "null", lb_t, lb_1);
            ExpItem* e2 = parse_Exp(n->children[2], "null", lb_t, lb_f);
            ret->tac.insert(ret->tac.end(), e1->tac.begin(), e1->tac.end());
            ret->tac.push_back(new Record(Record::R_LABEL, 1, lb_1));
            ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());
        } else if (op2 == "LT" || op2 == "LE" || op2 == "GT" || op2 == "GE" || op2 == "NE" || op2 == "EQ" ) {
            string np1 = new_place();
            string np2 = new_place();
            ExpItem* e1 = parse_Exp(n->children[0], np1);
            ExpItem* e2 = parse_Exp(n->children[2], np2);
            ret->tac.insert(ret->tac.end(), e1->tac.begin(), e1->tac.end());
            ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());

            if (op2 == "LT") ret->tac.push_back(new Record(Record::R_LT, 3, np1, np2, lb_t));
            else if (op2 == "LE") ret->tac.push_back(new Record(Record::R_LE, 3, np1, np2, lb_t));
            else if (op2 == "GT") ret->tac.push_back(new Record(Record::R_GT, 3, np1, np2, lb_t));
            else if (op2 == "GE") ret->tac.push_back(new Record(Record::R_GE, 3, np1, np2, lb_t));
            else if (op2 == "NE") ret->tac.push_back(new Record(Record::R_NE, 3, np1, np2, lb_t));
            else if (op2 == "EQ") ret->tac.push_back(new Record(Record::R_EQ, 3, np1, np2, lb_t));

            ret->tac.push_back(new Record(Record::R_GOTO, 1, lb_f));
        } else if (op2 == "PLUS" || op2 == "MINUS" || op2 == "MUL" || op2 == "DIV") {
            string t1 = new_place();
            string t2 = new_place();
            ExpItem* e1 = parse_Exp(n->children[0], t1);
            ExpItem* e2 = parse_Exp(n->children[2], t2);
            ret->tac.insert(ret->tac.end(), e1->tac.begin(), e1->tac.end());
            ret->tac.insert(ret->tac.end(), e2->tac.begin(), e2->tac.end());
            Record* r;
            if (op2 == "PLUS") r = new Record(Record::R_PLUS, 3, place, t1, t2);
            else if (op2 == "MINUS") r = new Record(Record::R_MINUS, 3, place, t1, t2);
            else if (op2 == "MUL") r = new Record(Record::R_MUL, 3, place, t1, t2);
            else if (op2 == "DIV") r = new Record(Record::R_DIV, 3, place, t1, t2);
            ret->tac.push_back(r);
        } else if (op2 == "LB") {
            
        } else if (op2 == "DOT") {
            
        }
    } else if (op1 == "LP") {

    } else if (op1 == "MINUS") {
        string np = new_place();
        ExpItem* e1 = parse_Exp(n->children[1], np);
        ret->tac.insert(ret->tac.begin(), e1->tac.begin(), e1->tac.end());
        ret->tac.push_back(new Record(Record::R_MINUS, 3, place, "#0", np));
    } else if (op1 == "NOT") {
        ExpItem* e = parse_Exp(n->children[1], lb_f, lb_t);
        ret->tac.insert(ret->tac.end(), e->tac.begin(), e->tac.end());
    } else if (op1 == "ID") {
        ret->tac.push_back(new Record(Record::R_ID, 2, place, n->children[0]->text));
        ret->id = n->children[0]->text;
    } else if (op1 == "INT") {
        ret->tac.push_back(new Record(Record::R_INT, 2, place, n->children[0]->text));
        Record r(Record::R_INT, 2, place, n->children[0]->text);
    } else if (op1 == "FLOAT") {

    } else if (op1 == "CHAR") {

    }

    return ret;
}