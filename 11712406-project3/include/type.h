#ifndef __TYPE_H
#define __TYPE_H

#include <string>
#include <vector>

using namespace std;

class Type;
class Array;
class Field;

class Type {

public:
    string name;
    enum {PRIMITIVE, ARRAY, STRUCTURE} category;
    union {
        string primitive;
        Array *array;
        vector<Field*>* fields;
    };

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

#endif