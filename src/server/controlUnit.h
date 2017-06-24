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
#include "../common/messenger.h"
#include "factory.h"
#include "territory.h"
#include "team.h"
#include "command.h"
#include "commandMonitor.h"

class Command;
class CommandMonitor;

class ControlUnit {
private:
    std::map<int,Unit*>& all_units;
    std::vector<Territory*>& territories;
    std::vector<Occupant*>& all_occupants;
    std::vector<Messenger*> players;
    CommandMonitor &commands;
    std::mutex m;
    bool winning;
    std::vector<Team>& teams;
    std::vector<Bullet*> all_bullets;
    std::vector<Unit> changed_units;
    std::vector<Occupant> changed_occupants;
    std::vector<Factory> changed_factories;
//    std::vector<Unit*> eliminated_units;
    int objects_counter;

public:
    ControlUnit(std::vector<Messenger *> &new_players,
                    std::map<int, Unit *> &all_units,
                    std::vector<Occupant *> &all_occupants,
                    std::vector<Team> &teams, CommandMonitor &commands,
                    std::vector<Territory *>& territories);

    // Method to start checking commands from players
    void run();

    void sleepFor(std::chrono::duration<double> sec);

    // Meant to make every unit make a micro action on the Tic
    void unitsMakeMicroAction();

    // Checks if any Occupant is dead. If so, it will remove it from the game
    void checkAllLivingOccupants();

    void moveAllBullets();

    // Command move unit. Meant to give the order to the unit to start moving
    // to de (x,y) position
    void cmdMoveUnit(const std::string& id_player, int id, int x, int y);

    void cmdAttack(const std::string& attacker_team, int id_unit, int target);

    void cmdGrab(const std::string& id_player, int id_unit, int target);

    void cmdFactoryCreate(const std::string& player_id, int id_factory);

    void cmdFactoryNext(const std::string& player_id, int id_factory);

    void cmdFactoryPrev(const std::string& player_id, int id_factory);

    void cmdFactoryCurrent(const std::string& player_id, int id_factory);

private:
    // Process all commands on commands vector and leaves the vector empty
    void executeCommands();

    void sendUpdateMessage();

    void sendMessageTo(const std::string& player_id, std::string& msg);

    std::string getUpdateInfo();

    std::string getInfoFromUnit(Unit& unit);

    std::string getInfoFromOccupant(Occupant& Occupant);

    std::string getInfoFromTerritory(Territory& territory);

    std::string getInfoFromBullets(Bullet& bullet);

    std::string getInfoFromFactories(Factory& factory);

    std::string getInfoFromUnitMold(UnitMold& mold, double time);

    std::string getInfoForAddUnit(Unit& unit);

    void makeTerritoriesChecks();

    void makeFactoryChecks();

    void checkForWinner();

    void sendFinnalMessage();

    void getTime(int& minutes, int& seconds, double time);
};


#endif //Z_TPGRUPAL_CONTROUNIT_H
