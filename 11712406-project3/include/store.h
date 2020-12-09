#ifndef __STORE_H
#define __STORE_H

#include "node.h"
#include "type.h"

#include <vector>
#include <list>
#include <string>
#include <unordered_map>

using namespace std;

class Item {

public:

    enum CATEGORY {
        I_FUNCTION,
        I_VARIABLE,
        I_STRUCT,
    } category;

    union { 
        vector<Type*>* types;
        vector<Field*>* fields;
    };

    Item();

};

class Store {
private:
    void ensure_table_entry(string name);

public:
    vector<vector<string>> scope;
    unordered_map<string, list<Item*>> table;
    
    Store();
    Item* lookup(string);
    Item* lookup(string, Item::CATEGORY);
    void insert_var(string, Type*);
    void insert_fun(string, vector<Type*>*);
    void insert_struct(string, vector<Field*>*);
    void insert(string, Item*);
    void remove(string);
    void add_scope();
    void sub_scope();

};


#endif