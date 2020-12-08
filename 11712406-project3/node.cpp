#include "node.h"
#include <string>
#include <cstdarg>

using namespace std;

Node::Node() : Node("", "", 0) {}
Node::Node(const string& name) : Node(name, "", 0) {}
Node::Node(const string& name, const string& text) : Node(name, text, 0) {}
Node::Node(const string& name, const string& text, int line_no) : name(name), text(text), line_no(line_no) {}

void Node::insert(int num, ...) {
    va_list list;
    va_start(list, num);

    for (int i = 0; i < num; ++i) {
        Node* p = va_arg(list, Node*);
        if (p->name.empty()) continue;
        this->children.push_back(p);
    }

    va_end(list);
}

void Node::show(int indent) {
    while (indent--) printf("  ");
    printf("%s", this->name.c_str());
    if (!this->children.empty()) {
        printf(" (%d)", this->line_no);
    }
    if (!this->text.empty()) {
        printf(": ");
        if (this->text == "INT") {
            printf("%d", atoi(this->text.c_str()));
        } else if (this->text == "FLOAT") {
            printf("%g", atof(this->text.c_str()));
        } else {
            printf("%s", this->text.c_str());
        }
    }
    printf("\n");
}

void Node::pre_traverse(int depth) {
    this->show(depth);
    for (auto child: this->children) {
        child->pre_traverse(depth + 1);
    }
}