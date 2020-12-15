#ifndef __OPTIMIZER_H
#define __OPTIMIZER_H

#include <vector>

#include "type.h"

using namespace std;


class Optimizer {
public:
    vector<Record>* codes;

    Optimizer();

    void optimize(vector<Record>*);
    void reduce_constant();
    void reduce_transmission();
    void reduce_label();
    void reduce_variable();

};

#endif