//
// Created by rodian on 22/05/17.
//

#include "controUnit.h"

ControUnit::ControUnit() {}

void ControUnit::connect(Messenger *new_player) {
    Lock l(m);
    this->players.push_back(new_player);
}
