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
private:
    int _size = 0;

public:
    string name;
    enum CATEGORY{
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

    Type();
    Type(string primitive);
    Type(Array* array);
    Type(Struct* structure);
    Type(Function* func);
    bool operator== (const Type& t) const;
    bool isTypeOf(const string& name) const;
    int size();
};

class Array {

public:
    Type* base;
    int length;

    Array();
    Array(Type* base, int lenght);
    bool operator== (const Array& arr) const;

};

class Field {

public:
    string name;
    Type* type;

    Field();
    Field(string name, Type* type);
};

class Struct {

public:
    string name;
    vector<Field*>* fields;

    Struct();
    Struct(string name, vector<Field*>* fields);

    int offset_of(string field);
};

class Function {

public:
    string name;
    Type* ret;
    vector<Field*>* args;

    Function();
    Function(string name, Type* ret, vector<Field*>* args);
};

#endif