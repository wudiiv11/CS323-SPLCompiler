#include "../include/type.h"

Type::Type() {}


Type::Type(string* primitive) {
    category = T_PRIMITIVE;
    this->primitive = primitive;
}


Type::Type(Array* array) {
    category = T_ARRAY;
    this->array = array;
}


Type::Type(Struct* structure) {
    category = T_STRUCTURE;
    this->structure = structure;
}


Type::Type(Function* func) {
    category = T_FUNCTION;
    this->funcion = func;
}


int Type::size() {
    if (_size == 0) {
        switch (this->category) {
        case T_STRUCTURE:
            for (auto i : *this->structure->fields) {
                _size += i->type->size();
            }
            break;
        case T_ARRAY:
            _size = this->array->base->size() * this->array->length;
            break;
        default:
            _size = 4;
            break;
        }
    }
    return _size;
}


Array::Array() {}


Array::Array(Type* base, int length) {
    this->base = base;
    this->length = length;
}


Field::Field() {}


Field::Field(string name, Type* type) {
    this->name = name;
    this->type = type;
}


Struct::Struct(string name, vector<Field*>* fields) {
    this->name = name;
    this->fields = fields;
}

int Struct::offset_of(string field) {
    int offset = 0;
    for (auto i : *fields) {
        if (i->name == field) {
            return offset;
        }
        offset += i->type->size();
    }
    return -1;
}


Type* Struct::type_of(string name) {
    for (auto s : *fields) {
        if (s->name == name) {
            return s->type;
        }
    }
    return nullptr;
}


Function::Function() {}


Function::Function(string name, Type* ret, vector<Field*>* args) {
    this->name = name;
    this->ret = ret;
    this->args = args;
}


Expr::Expr() {
    is_pointer = 0;
    t = nullptr;
}


string Record::to_string() {
    switch (category) {
    case R_INT:
        return args[0] + " := #" + args[1];
    case R_ID:
        return args[0] + " := " + args[1];
    case R_ASSIGN:
        return args[0] + " := " + args[1];
    case R_OFFSET:
        return args[0] + " := " + args[1] + " + #" + args[2];
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
        return "LABEL " + args[0] + " :";
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
        return args[0] + " := CALL " + args[1];
    case R_ARG:
        return "ARG " + args[0];
    case R_DEC:
        return "DEC " + args[0] + " " + args[1];
    default:
        return "error";
    }
}


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