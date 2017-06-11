//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_CONTROUNIT_H
#define Z_TPGRUPAL_CONTROUNIT_H

#include <iostream>
#include <chrono>
#include <thread>
#include "unit.h"
#include "../common/Lock.h"
#include "command.h"
#include "../common/messenger.h"
#include "factory.h"
#include "territory.h"

class Command;

class ControlUnit {
private:
    std::map<int,Unit>& all_units;
    std::vector<Territory> territories;
//    std::vector<Unit>& all_units;
    std::vector<Occupant>& all_occupants;
    std::vector<Messenger*> players;
    std::vector<Command>* commands;
    std::mutex m;
    bool winning;
    std::vector<Bullet*> all_bullets;
    std::vector<Unit>* changed_units;
    std::vector<Occupant>* changed_occupants;

public:
    ControlUnit(std::vector<Messenger *> &new_players,
                    std::map<int, Unit> &all_units,
                    std::vector<Occupant> &all_occupants,
                    std::vector<Territory> &territories);

    // Method to start checking commands from players
    void run();

    void sleepFor(double sec);

    // Meant to make every unit make a micro action on the Tic
    void unitsMakeMicroAction();

    // Checks if any Occupant is dead. If so, it will remove it from the game
    void checkAllLivingOccupants();

    void moveAllBullets();

    // Command move unit. Meant to give the order to the unit to start moving
    // to de (x,y) position
    void cmdMoveUnit(int id, int x, int y);

    void cmdAttack(std::string attacker_team, int id_unit, int target);

private:
    // Process all commands on commands vector and leaves the vector empty
    void executeCommands();

    void sendUpdateMessage();

    std::string getUpdateInfo();

    bool differenceOnUnits(Unit& x, Unit& y);

    std::string getInfoFromUnit(Unit& unit);

    std::string getInfoFromOccupant(Occupant& Occupant);
};


#endif //Z_TPGRUPAL_CONTROUNIT_H
