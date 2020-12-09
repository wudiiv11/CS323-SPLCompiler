#ifndef __TYPE_H
#define __TYPE_H

#include <string>
#include <vector>

using namespace std;

class Type;
class Array;
class Field;
class Struct;
class Function;

class Type {

public:
    string name;
    enum {
        T_PRIMITIVE, 
        T_ARRAY, 
        T_STRUCTURE,
        T_FUNCTION,
    } category;
    union {
        string primitive;
        Array *array;
        Struct* structure;
        Function *funcion;
    };

    Type(){}
    Type(string primitive);
    Type(Array* array);
    Type(string name, vector<Field*>* fields);
    bool operator== (const Type& t) const;
    bool isTypeOf(const string& name) const;
};

class Array {

public:
    Type* base;
    int size;
    Array(Type* base, int size);
    bool operator== (const Array& arr) const;

};

class Field {

public:
    string name;
    Type* type;

    Field(string name, Type* type) : name(name), type(type) {}
};

class Struct {

public:
    string name;
    vector<Field*>* fields;

    Struct(){}
    Struct(string name, vector<Field*>* fields) : name(name), fields(fields) {}
};

class Function {

public:
    string name;
    Type* ret;
    vector<Field*>* args;

    Function(){}
    Function(string name, Type* ret, vector<Field*>* args) : name(name), ret(ret), args(args) {}
};

#endif