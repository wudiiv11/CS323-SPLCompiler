#include "../include/exp_item.h"

ExpItem::ExpItem() {
    isLeftValue = false;
    type = nullptr;
    tac;
}

ExpItem::ExpItem(string id) {
    this->id = id;
}