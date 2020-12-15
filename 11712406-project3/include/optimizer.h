#ifndef __OPTIMIZER_H
#define __OPTIMIZER_H

#include <vector>

#include "type.h"

using namespace std;

class Optimizer;
class DAG;


class DAG {
    vector<Record>* codes;

    void reduce_constant();
    void reduce_transmission();
    void reduce_label();

public:

    DAG(vector<Record>*);

    void minimize();
};


class Optimizer {
private:
    DAG* graph;

public:

    Optimizer();

    void optimize(vector<Record>*);

};

#endif