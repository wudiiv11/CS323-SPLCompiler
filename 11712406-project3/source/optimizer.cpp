#include "../include/optimizer.h"


#include <unordered_map>
#include <iostream>


Optimizer::Optimizer() {
    codes = nullptr;
}


void Optimizer::optimize(vector<Record>* _codes) {
    this->codes = new vector<Record>(*_codes);
    int pre_size = this->codes->size();
    while (1) {
        reduce_constant();
        reduce_label();
        // reduce_transmission();
        if (pre_size == this->codes->size())
            break;
        pre_size = this->codes->size();
    }
}


void Optimizer::reduce_constant() {
    vector<Record>* new_codes = new vector<Record>();
    for (auto i : *codes) {
        if ((i.category == Record::R_PLUS || i.category == Record::R_MINUS || i.category == Record::R_MUL || i.category == Record::R_DIV) && i.args[1].at(0) == '#' && i.args[2].at(0) == '#') {
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
                cout << "error in reduce constant!" << endl;
                break;
            }
            new_codes->push_back(Record(Record::R_ASSIGN, 2, i.args[0], "#" + to_string(res)));
        } else {
            new_codes->push_back(i);
        }
    }
    delete codes;
    codes = new_codes;
}


void Optimizer::reduce_label() {
    unordered_map<string, string> parents;
    for (int i = 0; i < (*codes).size(); ++i) {
        if (codes->at(i).category != Record::R_LABEL)
            continue;
        if (i > 0 && codes->at(i - 1).category == Record::R_LABEL) 
            parents[codes->at(i).args[0]] = parents[codes->at(i - 1).args[0]];
        else
            parents[codes->at(i).args[0]] = codes->at(i).args[0];
    }
    
    vector<Record>* new_codes = new vector<Record>();
    for (auto i : *codes) {
        if (i.category == Record::R_LABEL) {
            if (i.args[0] == parents[i.args[0]])
                new_codes->push_back(i);
        } else if (i.category == Record::R_LT) {
            new_codes->push_back(Record(Record::R_LT, 3, i.args[0], i.args[1], parents[i.args[2]]));
        } else if (i.category == Record::R_LE) {
            new_codes->push_back(Record(Record::R_LE, 3, i.args[0], i.args[1], parents[i.args[2]]));
        } else if (i.category == Record::R_GT) {
            new_codes->push_back(Record(Record::R_GT, 3, i.args[0], i.args[1], parents[i.args[2]]));
        } else if (i.category == Record::R_GE) {
            new_codes->push_back(Record(Record::R_GE, 3, i.args[0], i.args[1], parents[i.args[2]]));
        } else if (i.category == Record::R_NE) {
            new_codes->push_back(Record(Record::R_NE, 3, i.args[0], i.args[1], parents[i.args[2]]));
        } else if (i.category == Record::R_EQ) {
            new_codes->push_back(Record(Record::R_EQ, 3, i.args[0], i.args[1], parents[i.args[2]]));
        } else if (i.category == Record::R_GOTO) {
            new_codes->push_back(Record(Record::R_GOTO, 1, parents[i.args[0]]));
        } else {
            new_codes->push_back(i);
        }
    }

    delete codes;
    codes = new_codes;
}


void Optimizer::reduce_transmission() {

}


