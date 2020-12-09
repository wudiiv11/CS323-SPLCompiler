#include "../include/store.h"

Item::Item() {}


Store::Store() { 
    scope;
    table; 
}


Item* Store::lookup(string str) {
    if (table.find(str) == table.end() || table[str].empty())
        return nullptr;
    return table[str].front();
}


Item* Store::lookup(string str, Item::CATEGORY c) {
    if (table.find(str) == table.end() || table[str].empty())
        return nullptr;
    for (auto i : table[str])
        if (i->category == c)
            return i;
    return nullptr;
}


void Store::ensure_table_entry(string name) {
    if (table.find(name) == table.end())
        table[name] = list<Item*>();
}


void Store::insert_var(string name, Type* type) {
    ensure_table_entry(name);
    Item* item = new Item();
    vector<Type*> types;
    item->types->push_back(type);
    item->category == Item::I_VARIABLE;
    insert(name, item);
}


void Store::insert_fun(string name, vector<Type*>* args) {
    ensure_table_entry(name);
    Item* item = new Item();
    item->types = args;
    item->category == Item::I_FUNCTION;
    insert(name, item);
}


void Store::insert_struct(string name, vector<Field*>* fields) {
    ensure_table_entry(name);
    Item* item = new Item();
    item->fields = fields;
    item->category == Item::I_STRUCT;
    insert(name, item);
}


void Store::insert(string name, Item* item) {
    ensure_table_entry(name);
    table[name].push_front(item);
    scope.back().push_back(name);
}


void Store::remove(string name) {
    table[name].pop_front();
}


void Store::add_scope() {
    scope.push_back(vector<string>());
}


void Store::sub_scope() {
    vector<string> v = scope.back();
    scope.pop_back();
    for (auto k : v)
        table[k].pop_front();
}