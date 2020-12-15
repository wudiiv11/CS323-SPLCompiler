#include "../include/optimizer.h"


DAG::DAG(vector<Record>* codes) {
    this->codes = codes;
}


void DAG::reduce_constant() {
    vector<Record>* new_codes;
    for (auto i : *codes) {
        if (i.args.size() != 3 || i.args[1].at(0) != '#' || i.args[2].at(0) != '#') {
            int t0 = atoi(i.args[1].substr(1).c_str());
            int t1 = atoi(i.args[2].substr(1).c_str());
            int res;
            switch (i.category) {
            case Record::R_PLUS:
                res = t0 + t1;
                break;
            case Record::R_MINUS:
                res = t0 - t1;
                break;
            case Record::R_MUL:
                res = t0 * t1;
                break;
            case Record::R_DIV:
                res = t0 / t1;
                break;
            default:
                res = -1;
                break;
            }
            new_codes->push_back(Record(Record::R_ASSIGN, 2, i.args[0], to_string(res)));
        } else {
            new_codes->push_back(i);
        }
    }
    codes = new_codes;
}


void DAG::reduce_label() {

}


void DAG::reduce_transmission() {

}


void DAG::minimize() {
    int pre_size = codes->size();
    while (1) {
        reduce_constant();
        reduce_label();
        reduce_transmission();
        if (pre_size == codes->size())
            break;
        pre_size = codes->size();
    }
}


Optimizer::Optimizer() {
    graph = nullptr;
}


void Optimizer::optimize(vector<Record>* codes) {
    graph = new DAG(codes);
    graph->minimize();
}