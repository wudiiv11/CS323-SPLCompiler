#include <string>

using namespace std;

#ifndef __TAC_H
#define __TAC_H

class tac {
public:
    string target;
    string op;
    string arg1;
    string arg2;

    tac(const string& t, const string& op, const string& a1, const string& a2) : target(t), op(op), arg1(a1), arg2(a2) {}
private:
};

#endif