#ifndef __SYNTAX_TABLE__
#define __SYNTAX_TABLE__

#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include "node.h"

#define semantic_error(type, line, msg) \
        string output = msg;\
        printf("Error type %d at Line %d: %s\n", type, line, output.c_str());

using namespace std;

class Array;
class Type;
class Field;
class Entry;

class Item{
public:
    int scope_depth;
    enum CATEGORY {VARIABLE, FUNCITON, STRUCTURE} category;
    // function items
    Type* ret;
    vector<Type*>* args;

    // variable type
    Type* type;
    Item(Type* ret, vector<Type*>* args, int depth);
    Item(Type* type, int depth);
};

class Entry {
public:
    string name;
    list<Item*>* items;
    void push_item(Item* item);
    void pop_item();

    Entry(string& name);
};

class SymbolTable {
public:
    int scope_depth;
    unordered_map<string, Entry*> table;
    void  add_scope();
    void  del_scope();
    Item* look_up(const string& name, Item::CATEGORY category) const;
    void  insert_item(string& name, Item* item);
    void  insert_struct(int line_no, string& name, vector<Field*>* type);
    void  insert_var(int line_no, string&name, Type* type);
    void  insert_func(int line_no, string& name, Type* ret, vector<Type*>* args);
    void  remove_item();
    void  remove_struct();
    void  remove_var(const string& name);
    void  remove_func();
};

#endif