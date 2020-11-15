#include "semantic_analyser.h"
#include <vector>

using namespace std;

void SemanticAnalyser::parse_semantic(Node* root) {
    parse_Program(root);
}

void SemanticAnalyser::parse_Program(Node* root) {
    parse_ExtDefList(root->children[0]);
}

void SemanticAnalyser::parse_ExtDefList(Node* root) {
    if (root->children.size() > 0) 
        parse_ExtDef(root->children[0]);
    if (root->children.size() > 1) 
        parse_ExtDefList(root->children[1]);
}

void SemanticAnalyser::parse_ExtDef(Node*root) {
    Type* type = parse_Specifier(root->children[0]);
    Node* node = root->children[1];
    if (node->name == "ExtDecList")
        parse_ExtDecList(node, type);
    else if (node->name == "FunDec") {
        parse_FunDec(node, type);
        parse_CompSt(root->children[2], type);
    }
}

void SemanticAnalyser::parse_ExtDecList(Node* root, Type* type) {
    parse_VarDec(root->children[0], type);
    if (root->children.size() > 2)
        parse_ExtDecList(root->children[2], type);
}

Type* SemanticAnalyser::parse_Specifier(Node* root) {
    Node* child = root->children[0];
    if(child->name == "TYPE") 
        return new Type(child->text);
    if(child->name == "StructSpecifier") 
        return parse_StructSpecifier(child);
    return NULL;
}

Type* SemanticAnalyser::parse_StructSpecifier(Node* root) {
    Type* type;
    if (root->children.size() == 2) {
        Item* item = table.look_up(root->children[1]->text, Item::STRUCTURE);
        if (item == NULL) {
            semantic_error(14, root->children[1]->line_no, "access undefined structure");
        } else {
            type = new Type(root->children[1]->text, item->type->fields);
        }
    } else if (root->children.size() == 5) {
        vector<Field*>* fields = new vector<Field*>();
        parse_DefList(root->children[3], fields);
        table.insert_struct(root->children[1]->line_no, root->children[1]->text, fields);
        type = new Type(root->children[1]->name, fields);
    }
    return type;
}

Field* SemanticAnalyser::parse_VarDec(Node* root, Type* type) {
    Node* node = root->children[0];
    if (node->name == "ID") {
        table.insert_var(node->line_no, node->text, type);
        return new Field(node->text, type);
    } else if (node->name == "VarDec") {
        Array* arr = new Array(type, atoi(root->children[2]->text.c_str()));
        Type* type = new Type(arr);
        return parse_VarDec(root->children[0], type);
    }
}

void SemanticAnalyser::parse_FunDec(Node* root, Type* ret) {
    Node* node = root->children[0];
    vector<Type*>* args = new vector<Type*>();
    if(root->children[2]->name == "VarList") {
        parse_VarList(root->children[2], args);
    }
    table.insert_func(node->line_no, node->text, ret, args);
}

void SemanticAnalyser::parse_VarList(Node* root, vector<Type*>* params) {
    params->push_back(parse_ParamDec(root->children[0]));
    if (root->children.size() > 2)
        parse_VarList(root->children[2], params);
}

Type* SemanticAnalyser::parse_ParamDec(Node* root) {
    Type* type = parse_Specifier(root->children[0]);
    parse_VarDec(root->children[1], type);
    return type;
}

void SemanticAnalyser::parse_CompSt(Node* root, Type* type) {
    table.add_scope();
    if (root->children.size() == 3) {
        if (root->children[1]->name == "DefList") {
            parse_DefList(root->children[1], new vector<Field*>());
        } else {
            parse_StmtList(root->children[1], type);
        }
    } else if (root->children.size() == 4) {
        parse_DefList(root->children[1], new vector<Field*>());
        parse_StmtList(root->children[2], type);
    }
    table.del_scope();
}

void SemanticAnalyser::parse_StmtList(Node* root, Type* type) {
    parse_Stmt(root->children[0], type);
    if(root->children.size() > 1)
        parse_StmtList(root->children[1], type);
}

void SemanticAnalyser::parse_Stmt(Node* root, Type* type) {
    Node* node = root->children[0];
    if (node->name == "Exp") {
        parse_Exp(root->children[0]);
    } else if (node->name == "CompSt") {
        parse_CompSt(node, type);
    } else if (node->name == "RETURN") {
        ExpItem* exp = parse_Exp(root->children[1]);
        if (exp && exp->type && !(*(exp->type) == *type)) {
            semantic_error(8, root->line_no, "incompatible return type");
        }
    } else if (node->name == "IF") {
        ExpItem* exp = parse_Exp(root->children[2]);
        parse_Stmt(root->children[4], type);
        if (root->children.size() > 6)
            parse_Stmt(root->children[6], type);
    } else if (node->name == "WHILE") {
        parse_Exp(root->children[2]);
        parse_Stmt(root->children[4], type);
    }
}

void SemanticAnalyser::parse_DefList(Node* root, vector<Field*>* fields) {
    if (root->children[0]->name == "Def")
        parse_Def(root->children[0], fields);
    if (root->children.size() > 1)
        parse_DefList(root->children[1], fields);
}

void SemanticAnalyser::parse_Def(Node* root, vector<Field*>* fields) {
    Type* type = parse_Specifier(root->children[0]);
    parse_DecList(root->children[1], type, fields);
}

void SemanticAnalyser::parse_DecList(Node* root, Type* type, vector<Field*>* fields) {
    parse_Dec(root->children[0], type, fields);
    if (root->children.size() > 2)
        parse_DecList(root->children[2], type, fields);
}

void SemanticAnalyser::parse_Dec(Node* root, Type* type, vector<Field*>* fields) {
    Field* field = parse_VarDec(root->children[0], type);
    fields->push_back(field);
    if (root->children.size() > 2) {
        parse_Exp(root->children[2]);
    }
}

ExpItem* SemanticAnalyser::parse_Exp(Node* root) {
    string stuff = root->children[0]->name;
    Type* type = NULL;
    if (stuff == "Exp") {
        string oper = root->children[1]->name;
        if (oper == "ASSIGN") {
            ExpItem* exp1 = parse_Exp(root->children[0]);
            ExpItem* exp2 = parse_Exp(root->children[2]);
            if (!exp1->isLeftValue) {
                semantic_error(6, root->children[0]->line_no, "left side in assignment is rvalue");
            }
            if (exp1->type && exp2->type && *(exp1->type) == *(exp2->type) || !exp1->type && !exp2->type) {
                type = exp1->type;
            } else {
                semantic_error(5, root->children[0]->line_no, "unmatching type on both sides of assignment");
            }
            return new ExpItem(type, false);
        } else if (oper == "AND" || oper == "OR") {
            ExpItem* exp1 = parse_Exp(root->children[0]);
            ExpItem* exp2 = parse_Exp(root->children[2]);
            if (exp1->type->isTypeOf("int") && exp2->type->isTypeOf("int")) {
                type = exp1->type;
            } else {
                semantic_error(7, root->children[0]->line_no, "boolean operation on non-int variables");
            }
            return new ExpItem(type, false);
        } else if (oper == "LT" || oper == "LE" || oper == "GT" || oper == "GE" || oper == "NE" || oper == "EQ") {
            ExpItem* exp1 = parse_Exp(root->children[0]);
            ExpItem* exp2 = parse_Exp(root->children[2]);
            if (*(exp1->type) == *(exp2->type)) {
                type = exp1->type;
            } else {
                semantic_error(7, root->children[0]->line_no, "binary operation on non-number variables");
            }
            return new ExpItem(type, false);
        } else if (oper == "PLUS" || oper == "MINUS" || oper == "MUL" || oper == "DIV") {
            ExpItem* exp1 = parse_Exp(root->children[0]);
            ExpItem* exp2 = parse_Exp(root->children[2]);
            Type* type = NULL;
            if (exp1->type && exp2->type) {
                if (!(*(exp1->type) == *(exp2->type))) {
                    if (exp1->type->isTypeOf("int") && exp2->type->isTypeOf("float")
                        || exp1->type->isTypeOf("float") && exp2->type->isTypeOf("int")) {
                        type = new Type("float");
                    } else {
                        semantic_error(7, root->children[0]->line_no, "binary operation on non-number variables");
                        type = new Type(NULL);
                    }
                } else if (exp1->type->isTypeOf("int") || exp1->type->isTypeOf("float")) {
                    type = exp1->type;
                }
            }
            return new ExpItem(type, false);
        } else if (oper == "LB") {
            
        } else if (oper == "DOT") {
            ExpItem* exp = parse_Exp(root->children[0]);
            if (exp->type != NULL && exp->type->category == Type::STRUCTURE) {
                Type* type = NULL;
                for (auto field: *(exp->type->fields)) {
                    if (field->name == root->children[2]->text) {
                        type = field->type;
                        break;
                    }
                }
                if (type == NULL) {
                    semantic_error(14, root->children[0]->line_no, "no such member: " + root->children[2]->text);
                } 
            } else {
                semantic_error(14, root->children[0]->line_no, "no such member: " + root->children[2]->text);
            }
            return new ExpItem(type, true);
        }
    } else if (stuff == "LP") {
        return parse_Exp(root->children[1]);
    } else if (stuff == "MINUS") {
        ExpItem* exp = parse_Exp(root->children[1]);
        if (exp->type->isTypeOf("int") || exp->type->isTypeOf("float")){
            return new ExpItem(exp->type, false);
        } else {
            semantic_error(7, root->children[0]->line_no, "binary operation on non-number variables");
            return new ExpItem(NULL, false);
        }
    } else if (stuff == "NOT") {
        ExpItem* exp = parse_Exp(root->children[1]);
        if (!exp->type->isTypeOf("int")) {
            semantic_error(7, root->children[0]->line_no, "boolean operation on non-int variables");
        } else {
            type = exp->type;
        }
        return new ExpItem(type, false);
    } else if (stuff == "ID") {
        if (root->children.size() == 1) {
            Item* item = table.look_up(root->children[0]->text, Item::VARIABLE);
            if (item != NULL && item->category == item->VARIABLE) {
                type = item->type;
            } else {
                semantic_error(1, root->children[0]->line_no, "undefined varaible: " + root->children[0]->text);
            }
            return new ExpItem(type, true);
        } else {
            Node* node = root->children[0];
            vector<ExpItem*>* args = new vector<ExpItem*>();
            if (root->children[2]->name == "Args")
                parse_Args(root->children[2], args);
            Item* item = table.look_up(node->text, Item::FUNCITON);
            if (item == NULL) {
                semantic_error(2, node->line_no, "undefined function: " + node->text);
                return NULL;
            }
            if (item->category != Item::FUNCITON) {
                semantic_error(11, node->line_no, "invoking non-function variable: " + node->text);
                return NULL;
            }
            if (item->args->size() != args->size()) {
                semantic_error(9, node->line_no, "invalid argument number for compare, expect " + to_string(item->args->size()) + ", get " + to_string(args->size()));
                return NULL;
            }
            for (int i = 0; i < args->size(); ++i) {
                Type* a = item->args->at(i);
                Type* b = args->at(i)->type;
                if(!(*a == *b)) {
                    semantic_error(9, node->line_no, "function's args mismatch the declared para");
                    break;
                }
            }
            return new ExpItem(item->ret, false);
        }
    } else if (stuff == "INT" || stuff == "FLOAT" || stuff == "CHAR") {
        Type* type;
        if (stuff == "INT") {
            type = new Type("int");
        } else if (stuff == "FLOAT") {
            type = new Type("float");
        } else if (stuff == "CHAR") {
            type = new Type("char");
        }
        return new ExpItem(type, false);
    }
    return new ExpItem(type, false);
}

void SemanticAnalyser::parse_Args(Node* root, vector<ExpItem*>* args) {
    args->push_back(parse_Exp(root->children[0]));
    if (root->children.size() > 2) 
        parse_Args(root->children[2], args);
}

ExpItem::ExpItem(Type* type, bool isLeftValue) : type(type), isLeftValue(isLeftValue) {}