//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_CONTROUNIT_H
#define Z_TPGRUPAL_CONTROUNIT_H


#include "unit.h"
#include "messenger.h"

class ControUnit {
private:
    std::vector<Unit*> all_units;
    std::vector<Occupant*> all_occupants;
    std::vector<Messenger*> players;
    std::mutex m;

public:
    ControUnit();

    void connect(Messenger* new_player);
};


#endif //Z_TPGRUPAL_CONTROUNIT_H
