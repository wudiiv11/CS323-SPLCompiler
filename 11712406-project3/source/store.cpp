#include "../include/store.h"
#include <iostream>

Item::Item() {
    t = nullptr;
    is_pointer = 0;
}


Store::Store() {
    scope.push_back(new vector<string>());
}


Item* Store::lookup(string str) {

#ifdef __DEBUG_
    cout << "lookup " << str << endl;
#endif 

    if (table.find(str) == table.end() || table[str].empty())
        return nullptr;
    
#ifdef __DEBUG_
    cout << "found!" << endl;
#endif

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


void Store::insert(string name, string alias, Type* type, int is_pointer=false) {

#ifdef __DEBUG_
    cout << "insert " << name << endl;
#endif

    ensure_table_entry(name);
    Item* item = new Item();
    item->alias = alias;
    item->t = type;
    item->is_pointer = is_pointer;
    table[name].push_front(item);
    scope.back()->push_back(name);
}


void Store::add_scope() {

#ifdef __DEBUG_
    // cout << "add scope" << endl;
#endif

    scope.push_back(new vector<string>());
}


void Store::sub_scope() {

#ifdef __DEBUG_
    cout << "sub scope" << endl;
#endif

    vector<string>* v = scope.back();
    scope.pop_back();
    for (auto k : *v)
        table[k].pop_front();
    // delete居然报错了？？？
    // delete v;
}