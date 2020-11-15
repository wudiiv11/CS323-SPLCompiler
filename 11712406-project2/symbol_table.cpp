#include "symbol_table.h"
#include "node.h"
#include <string>
#include <vector>

using namespace std;

Type::Type(string primitive) : category(PRIMITIVE), primitive(primitive) {}
Type::Type(Array* array) : category(ARRAY), array(array) {}
Type::Type(string name, vector<Field*>* fields) : name(name), category(STRUCTURE), fields(fields) {}

bool Type::operator== (const Type& t) const {
    if (category != t.category) return false;
    if (category == PRIMITIVE) return primitive == t.primitive;
    if (category == ARRAY) return *array == *(t.array);
    return name == t.name;
}

bool Type::isTypeOf (const string& name) const {
    if (name == "int" || name == "float" || name == "char") {
        return category == PRIMITIVE && primitive == name;
    }
    return false;
}

Array::Array(Type* base, int size) : base(base), size(size) {}

bool Array::operator== (const Array& arr) const {
    return *base == *(arr.base) && size == arr.size;
}

void SymbolTable::add_scope() {
    ++scope_depth;
}

void SymbolTable::del_scope() {
    --scope_depth;
}

Item* SymbolTable::look_up(const string& name, Item::CATEGORY category) const {
    auto e = table.find(name);
    if (e == table.end()) return NULL;
    for (auto item: *(e->second->items)) {
        if (category == item->category) {
            return item;
        }
    }
    return e->second->items->front();
}

Item::Item(Type* ret, vector<Type*>* args, int depth) : ret(ret), args(args), scope_depth(depth) {
    category = Item::FUNCITON;
}

Item::Item(Type* type, int depth) : type(type), scope_depth(depth) {
    category = Item::VARIABLE;
}

Entry::Entry(string& name) : name(name) {
    items = new list<Item*>();
}

void Entry::push_item(Item* item) {
    items->push_front(item);
}

void Entry::pop_item() {
    items->pop_front();
}

void SymbolTable::insert_item(string& name, Item* item) {
    if (table.find(name) == table.end()) {
        table.emplace(name, new Entry(name));
    }
    Entry* entry = table.find(name)->second;
    entry->push_item(item);
}

void SymbolTable::remove_item() {

}

void SymbolTable::insert_struct(int line_no, string& name, vector<Field*>* fields) {
    auto e = table.find(name);
    if(e != table.end()) {
        Entry* entry = e->second;
        for (Item* item: *(entry->items)) {
            if (item->category == Item::STRUCTURE && item->scope_depth == scope_depth) {
                semantic_error(15, line_no, "redefine struct: " + name);
                return;
            }
        }
    }
    Type* type = new Type(name, fields);
    Item* item = new Item(type, scope_depth);
    insert_item(name, item);
}

void SymbolTable::remove_struct() {

}

void SymbolTable::insert_var(int line_no, string& name, Type* type) {
    auto e = table.find(name);
    if (e != table.end()) {
        Entry* entry = e->second;
        for (Item* item: *(entry->items)) {
            if (item->category == Item::VARIABLE && item->scope_depth == scope_depth) {
                semantic_error(3, line_no, "redefine variable: " + name);
                return;
            }
        }
    }
    Item* item = new Item(type, scope_depth);
    insert_item(name, item);
}

void SymbolTable::insert_func(int line_no, string& name, Type* ret, vector<Type*>* args) {
    auto e = table.find(name);
    if (e != table.end()) {
        Entry* entry = e->second;
        for (Item* item: *(entry->items)) {
            if (item->category == Item::FUNCITON) {
                semantic_error(4, line_no, "redefine function: " + name);
                return;
            }
        } 
    }
    Item* item = new Item(ret, args, scope_depth);
    insert_item(name, item);
}

void SymbolTable::remove_func() {

}