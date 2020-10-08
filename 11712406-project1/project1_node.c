#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct Node {
    char*   name;
    char*   text;
    struct Node*   child_list_head; /* dummy Head*/
    struct Node*   next;
    int     line_no;
} Node;

char* str_copy(const char* src) {
    if (!src) return NULL;
    int len = strlen(src);
    char* dest = malloc(len + 1);
    strncpy(dest, src, len);
    return dest;
}

Node* make_node(char* name, char* text, int line_no) {
    Node* node = (Node*) malloc (sizeof(Node));
    Node* child_head = (Node*) malloc (sizeof(Node));
    memset(node, 0, sizeof(Node));
    memset(child_head, 0, sizeof(Node));
    node->name = name;
    node->text = str_copy(text);
    node->line_no = line_no;
    node->child_list_head = child_head;
    return node;
}

void insert(int num, ...) {
    va_list list;
    va_start(list, num);
    Node* parent = va_arg(list, Node*);
    Node* p = parent->child_list_head;
    while (p->next) p = p->next;
    for (int i = 1; i < num; i++) {
        Node* child = va_arg(list, Node*);
        if (!strcmp(child->name, "NULL")) continue;
        p->next = child;
        p = p->next;
    }
    va_end(list);
}