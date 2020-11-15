#ifndef __NODE__
#define __NODE__

#include <vector>
#include <string>
using namespace std;

class Node {

private:

public:
    string name;
    string text;
    vector<Node*> children;
    int line_no;

    Node();
    Node(const string& name);
    Node(const string& name, const string& text);
    Node(const string& name, const string& text, int line_no);

    void insert(int num, ...);
    void show(int indent);
    void pre_traverse(int depth);

};

#endif