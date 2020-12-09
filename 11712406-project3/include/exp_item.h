#ifndef __EXP_ITEM_H
#define __EXP_ITEM_H

#include "type.h"
#include "record.h"

class ExpItem {
    
public:
    Type* type;
    bool isLeftValue;
    vector<Record*> tac;
    string id; // 如果是个变量, 则保存到id
    ExpItem();
    ExpItem(Type* type, string tac, bool isLeftValue = false);
    ExpItem(string id);
};

#endif