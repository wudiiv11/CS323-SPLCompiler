#include "parser.h"
#include <vector>

using namespace std;

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
    parse_Stmt(n->children[0], t);
    parse_StmtList(n->children[1], t);
}

void Parser::parse_Stmt(Node* n, Type* t) {
    if (n->children[0]->name == "Exp") {
        parse_Exp(n->children[0]);
    } else if (n->children[0]->name == "CompSt") {
        parse_CompSt(n->children[0], t);
    } else if (n->children[0]->name == "RETURN") {
        parse_Exp(n->children[1]);
    } else if (n->children[0]->name == "IF") {
        parse_Exp(n->children[2]);
        parse_Stmt(n->children[3], t);
        if(n->children.size() > 6)
            parse_Stmt(n->children[6], t);
    } else if (n->children[0]->name == "WHILE") {
        parse_Exp(n->children[2]);
        parse_Stmt(n->children[4], t);
    }
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
    string op1 = n->children[0]->name;
    if (op1 == "Exp") {
        string op2 = n->children[1]->name;
        if (op2 == "ASSIGN") {
            
        } else if (op2 == "AND") {

        } else if (op2 == "OR") {

        } else if (op2 == "LT") {
            
        } else if (op2 == "LE") {
            
        } else if (op2 == "GT") {
            
        } else if (op2 == "GE") {
            
        } else if (op2 == "NE") {
            
        } else if (op2 == "EQ") {
            
        } else if (op2 == "PLUS") {
            
        } else if (op2 == "MINUS") {
            
        } else if (op2 == "MUL") {
            
        } else if (op2 == "DIV") {
            
        } else if (op2 == "LB") {
            
        } else if (op2 == "DOT") {
            
        }
    } else if (op1 == "LP") {

    } else if (op1 == "MINUS") {

    } else if (op1 == "NOT") {

    } else if (op1 == "ID") {

    } else if (op1 == "INT") {

    } else if (op1 == "FLOAT") {

    } else if (op1 == "CHAR") {

    }
}