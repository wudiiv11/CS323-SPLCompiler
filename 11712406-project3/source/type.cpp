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
    int cnt = 0;
    for (auto i : *fields) {
        if (i->name == field) {
            return cnt << 2;
        }
        cnt += 1;
    }
    return -1;
}


Function::Function() {}


Function::Function(string name, Type* ret, vector<Field*>* args) {
    this->name = name;
    this->ret = ret;
    this->args = args;
}

