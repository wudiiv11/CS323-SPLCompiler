#include "../include/type.h"

Type::Type() {}


Type::Type(string primitive) {
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


Array::Array() {}


Array::Array(Type* base, int size) {
    this->base = base;
    this->size = size;
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


Function::Function() {}


Function::Function(string name, Type* ret, vector<Field*>* args) {
    this->name = name;
    this->ret = ret;
    this->args = args;
}

