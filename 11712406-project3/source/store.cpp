#include "../include/store.h"
#include <iostream>

Item::Item() {}


Store::Store() {
    scope.push_back(new vector<string>());
}


Item* Store::lookup(string str) {
    if (table.find(str) == table.end() || table[str].empty())
        return nullptr;
    return table[str].front();
}


Item* Store::lookup(string str, Type::CATEGORY c) {
    if (table.find(str) == table.end() || table[str].empty())
        return nullptr;
    for (auto i : table[str])
        if (i->t->category == c)
            return i;
    return nullptr;
}


void Store::ensure_table_entry(string name) {
    if (table.find(name) == table.end())
        table[name] = list<Item*>();
}


void Store::insert(string name, string alias, Type* type) {
    ensure_table_entry(name);
    Item* item = new Item();
    item->alias = alias;
    item->t = type;
    table[name].push_front(item);
    scope.back()->push_back(name);
}


void Store::add_scope() {
    scope.push_back(new vector<string>());
}


void Store::sub_scope() {
    vector<string>* v = scope.back();
    scope.pop_back();
    for (auto k : *v)
        table[k].pop_front();
    // delete居然报错了？？？
    // delete v;
}