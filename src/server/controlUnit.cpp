//
// Created by rodian on 22/05/17.
//

#include "controlUnit.h"
#define WAIT 0.5


ControlUnit::ControlUnit(std::vector<Messenger *> &new_players,
                         std::map<int, Unit> &all_units,
                         std::vector<Occupant*> &all_occupants,/*
                         std::vector<Territory> &territories,*/
                         std::vector<Team>& teams) :
    all_units(all_units), /*territories(territories),*/
    all_occupants(all_occupants), players(new_players),
    winning(false), teams(teams) {
    this->changed_units = new std::vector<Unit>;
}

void ControlUnit::run() {
    while(!winning) {
        double t3(WAIT);
        changed_units->clear();
        changed_occupants.clear();
        auto it = all_occupants.begin();
        // Copy starting state of Occupants
        changed_occupants = all_occupants;
        auto t1 = std::chrono::high_resolution_clock::now();

        // execute commands
        executeCommands();

        // do stuff
        this->unitsMakeMicroAction();
        this->moveAllBullets();
        this->makeTerritoriesChecks();
        this->checkAllLivingOccupants();

        //send update message
        this->sendUpdateMessage();

//        this->checkForWinner();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span =
             std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << t3 - time_span.count() << std::endl;
        sleepFor(t3 - time_span.count());
    }
    // send victory or defeated message
    this->sendFinnalMessage();
}

void ControlUnit::sleepFor(double msec) {
    std::this_thread::sleep_for(std::chrono::duration<double> (msec));
}

void ControlUnit::unitsMakeMicroAction() {
    for (auto x: all_units){
        Unit y = x.second;
        x.second.makeAction();
        if (differenceOnUnits(x.second,y)) {
            changed_units->push_back(x.second);
        }
        if (!x.second.areYouAlive()) {
            all_units.erase(x.first);
        } else {
            std::vector<Bullet*> tmp = x.second.collectBullets();
            all_bullets.insert(all_bullets.end(),tmp.begin(),tmp.end());
        }
    }
}


void ControlUnit::checkAllLivingOccupants() {
    std::vector<Occupant*>::iterator it = changed_occupants.begin();
    std::cout<< changed_occupants.size() << std::endl;
    int i = 0;
    for (; it != changed_occupants.end();) {
        if (all_occupants[i]->getLifeLeft() ==
                (*it)->getLifeLeft()) {
            it = changed_occupants.erase(it);
        } else {
            ++it;
        }
        ++i;
    }
    std::cout<< changed_occupants.size() << std::endl;
    // if dead erase Occupant
    it = all_occupants.begin();
    for(;it != all_occupants.end();){
        if(!(*it)->areYouAlive()) {
            //erase it from map
            it = all_occupants.erase(it);
            // if building put ruins
        } else {
            ++it;
        }
    }
}

void ControlUnit::cmdMoveUnit(int id, int x, int y) {
    std::map<int,Unit>::iterator it;
    it = all_units.find(id);
    (*it).second.calculateRoadTo(x,y);
}

void ControlUnit::executeCommands() {
    std::vector<Command> commands_copy;
    m.lock();
    for (Command x: commands) {
        commands_copy.push_back(x);
    }
    commands.clear();
    m.unlock();

    // Execute command
    for (auto cmd: commands_copy) {
        cmd();
    }
}

void ControlUnit::sendUpdateMessage() {
    std::string info = getUpdateInfo();
    for (auto y: players) {
        y->sendMessage(info);
    }
}

std::string ControlUnit::getUpdateInfo() {
    std::string  update_msg = "update-";
    for (auto z: *changed_units) {
        update_msg += getInfoFromUnit(z);
    }

    for (auto y: changed_occupants) {
        update_msg += getInfoFromOccupant(y);
    }

//    for(auto t: territories) {
//       if (t.doesTerritorysOwnerChanged()) {
//           update_msg += getInfoFromTerritory(t);
//       }
//    }

    return update_msg;
}

bool ControlUnit::differenceOnUnits(Unit &x, Unit &y) {
    bool differ = false;
    if (x.getState() != y.getState())
        differ = true;
    if (x.getTeam() != y.getTeam()) // For neutral vehicules
        differ = true;
    if (x.getLifeLeft() != y.getLifeLeft())
        differ = true;
    if ((x.getCurrentPosition().getX() != y.getCurrentPosition().getX()) ||
            (x.getCurrentPosition().getY() != y.getCurrentPosition().getY()))
        differ = true;
    return differ;
}

std::string ControlUnit::getInfoFromUnit(Unit &unit) {
    std::string info = "";
    info += std::to_string(unit.getId()) + "-";
    info += unit.getState() + "-";
    info += std::to_string(unit.getCurrentPosition().getX()) + "-";
    info += std::to_string(unit.getCurrentPosition().getY()) + "-";
    info += std::to_string(unit.getLifeLeft()) + "--";
    return info;
}

std::string ControlUnit::getInfoFromOccupant(Occupant* Occupant) {
    std::string info = "";
    info += std::to_string(Occupant->getId()) + "-";
    info += std::to_string(Occupant->getPosition().getX()) + "-";
    info += std::to_string(Occupant->getPosition().getY()) + "-";
    info += std::to_string(Occupant->getLifeLeft()) + "--";
    return info;
}

std::string ControlUnit::getInfoFromTerritory(Territory &territory) {
    Position flag_pos = territory.getFlagPosition();
    std::string info = "flagOn-";
    info += std::to_string(flag_pos.getX()) + "-";
    info += std::to_string(flag_pos.getY()) + "-";
    info += territory.getTeam() + "--";
    return info;
}

void ControlUnit::cmdAttack(std::string attacker_team, int id_unit,
                            int target) {
    std::map<int,Unit>::iterator it;
    it = all_units.find(id_unit);
    Unit selected_unit = (*it).second;
    if (selected_unit.getTeam() == attacker_team) {
        for (auto z: all_occupants) {
            if (z->getId() == target) {
                if (z->getTeam() != attacker_team) {
//                    selected_unit.setTargetToAttack(z);
                    break;
                }
            }
        }
    }
}

void ControlUnit::moveAllBullets() {
    std::vector<Bullet*>::iterator it = all_bullets.begin();
    for (; it != all_bullets.end();) {
        (*it)->move();
        if ((*it)->didHit()) {
            it = all_bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void ControlUnit::makeTerritoriesChecks() {
//    for (auto t: territories) {
//        std::map<int,Factory>& fac = t.getFactories();
//        for (auto f: fac) {
//            f.second.build();
//        }
//    }
}

void ControlUnit::checkForWinner() {
    int teams_alive = 0;
    for (auto t: teams) {
        if (!t.doesTeamLose()) {
            teams_alive += 1;
        }
    }

    if (teams_alive == 1) {
        winning = true;
    }
}

void ControlUnit::sendFinnalMessage() {
    std::string winner = "winner-";
    for (auto t: teams) {
        if (!t.doesTeamLose()) {
            std::vector<PlayerInfo>& winners = t.getPlayersInfo();
            for (auto w: winners) {
                winner += w.getPlayerId();
            }
        }
    }
    for (auto y: players) {
        y->sendMessage(winner);
    }
}


