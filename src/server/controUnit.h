//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_CONTROUNIT_H
#define Z_TPGRUPAL_CONTROUNIT_H

#include <iostream>
#include <chrono>
#include <thread>
#include "unit.h"
#include "../messenger.h"
#include "../Lock.h"

typedef std::chrono::duration<int,std::milli> milliseconds_type;

class ControUnit {
private:
    std::vector<Unit*> all_units;
    std::vector<Occupant*> all_occupants;
    std::vector<Messenger*> players;
    std::mutex m;
    bool winning;

public:
    ControUnit();

    void connect(Messenger* new_player);

    void run();

    void sleepFor(milliseconds_type sec);

    void unitsMakeMicroAcction();

    void checkAllLivingOccupants();
};


#endif //Z_TPGRUPAL_CONTROUNIT_H
