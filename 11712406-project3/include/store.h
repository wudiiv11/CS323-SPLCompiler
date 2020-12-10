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

    Type* t;
    string alias;

    Item();

};

class Store {
private:
    void ensure_table_entry(string name);

public:
    vector<vector<string>> scope;
    unordered_map<string, list<Item*>> table;
    
    Store();
    Item* lookup(string, Type::CATEGORY c);
    void insert(string, string alias, Type* type);
    void add_scope();
    void sub_scope();

};


#endif