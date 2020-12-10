#include "../include/store.h"

Item::Item() {}


Store::Store() {}


Item* Store::lookup(string str, Type::CATEGORY c) {
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


void Store::insert(string name, string alias, Type* type) {
    ensure_table_entry(name);
    Item* item = new Item();
    
    // table[name].push_front(item);
    // scope.back().push_back(name);
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