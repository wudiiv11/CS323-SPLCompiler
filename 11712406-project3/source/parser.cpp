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
    if (n->children.size() > 0 && n->children[0]->name == "Stmt") {
        parse_Stmt(n->children[0], t);
        parse_StmtList(n->children[1], t);
    }
}

ExpItem* Parser::parse_Stmt(Node* n, Type* t) {
    ExpItem* ret = new ExpItem();
    
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


ExpItem* Parser::parse_Exp(Node* n) {
    ExpItem* ret = new ExpItem();
    

    return ret;
}