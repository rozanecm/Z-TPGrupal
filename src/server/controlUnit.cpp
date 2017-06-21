//
// Created by rodian on 22/05/17.
//

#include "controlUnit.h"
#define WAIT 0.2
#define BULLET "bullet"

ControlUnit::ControlUnit(std::vector<Messenger *> &new_players,
                         std::map<int, Unit *> &all_units,
                         std::vector<Occupant *> &all_occupants,
                         std::vector<Team> &teams, CommandMonitor &commands) :
    all_units(all_units), /*territories(territories),*/
    all_occupants(all_occupants), players(new_players), commands(commands),
    winning(false), teams(teams),objects_counter((int)all_occupants.size()) {
}

void ControlUnit::run() {
    while(!winning) {
        double t3(WAIT);

        auto t1 = std::chrono::high_resolution_clock::now();

        // execute commands
        executeCommands();

        // do stuff
        this->moveAllBullets();
        this->unitsMakeMicroAction();
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
        changed_units.clear();
        changed_occupants.clear();
    }
    // send victory or defeated message
    this->sendFinnalMessage();
}

void ControlUnit::sleepFor(double msec) {
    std::this_thread::sleep_for(std::chrono::duration<double> (msec));
}

void ControlUnit::unitsMakeMicroAction() {
    for (auto x: all_units){
        if ((*x.second).doYouNeedToDisappear()) {
            all_units.erase(x.first);
        } else {
            (*x.second).makeAction();
            if ((*x.second).haveYouChanged()) {
                changed_units.push_back((*x.second));
            }
            if (!(*x.second).areYouAlive()) {
                (*x.second).mustDisappear();
            }
            if ((*x.second).doYouHaveAnyBullets()) {
                std::vector<Bullet *> tmp = (*x.second).collectBullets();
                for (auto b: tmp) {
                    b->setCorrectId(objects_counter);
                    ++objects_counter;
                }
                all_bullets.insert(all_bullets.end(), tmp.begin(), tmp.end());
            }
        }
    }
}


void ControlUnit::checkAllLivingOccupants() {
    std::vector<Occupant*>::iterator it = all_occupants.begin();
    for(;it != all_occupants.end();){
        if((*it)->doYouNeedToDisappear()) {
            //erase it from map
            it = all_occupants.erase(it);
            // if building put ruins
        } else {
            if((*it)->haveYouChanged())
                changed_occupants.push_back(*(*it));
            if(!(*it)->areYouAlive()) {
                (*it)->mustDisappear();
            }
            ++it;
        }
    }
}

void ControlUnit::cmdMoveUnit(std::string& id_player,int id, int x, int y) {
    std::map<int,Unit*>::iterator it;
    it = all_units.find(id);
    if ((*it->second).getTeam() == id_player)
        (*it->second).calculateRoadTo(x,y);
}

void ControlUnit::cmdAttack(std::string attacker_team, int id_unit,
                            int target) {
    std::map<int,Unit*>::iterator it;
    it = all_units.find(id_unit);
    if ((*it->second).getTeam() == attacker_team) {
        for (auto z: all_occupants) {
            if (z->getId() == target) {
                if (z->getTeam() != attacker_team) {
                    (*it->second).setTargetToAttack(z);
                    break;
                }
            }
        }
    }
}

void ControlUnit::executeCommands() {
    std::vector<Command> commands_copy;
    commands.copyCommands(commands_copy);

    // Execute command
    for (auto cmd: commands_copy) {
        cmd();
    }
}

void ControlUnit::sendUpdateMessage() {
    std::string info = getUpdateInfo();
    if (!info.size()) {
        return;
    }
    info = "update-" + info;
    for (auto y: players) {
        y->sendMessage(info);
    }
}

std::string ControlUnit::getUpdateInfo() {
    std::string  update_msg = "";
    for (auto z: changed_units) {
        update_msg += getInfoFromUnit(z);
    }

    for (auto y: changed_occupants) {
        update_msg += getInfoFromOccupant(y);
    }

    for (auto b: all_bullets) {
        update_msg += getInfoFromBullets(*b) ;
    }

//    for(auto t: territories) {
//       if (t.doesTerritorysOwnerChanged()) {
//           update_msg += getInfoFromTerritory(t);
//       }
//    }

    return update_msg;
}

//bool ControlUnit::differenceOnUnits(Unit &x,std::string& state,
//                                    std::string& team,
//                                    int life, Position& pos) {
//    bool differ = false;
//    if (x.getActionState() != state)
//        differ = true;
//    if (x.getTeam() != team) // For neutral vehicules
//        differ = true;
//    if (x.getLifeLeft() != life)
//        differ = true;
//    if ((x.getCurrentPosition().getX() != pos.getX()) ||
//            (x.getCurrentPosition().getY() != pos.getY()))
//        differ = true;
//    return differ;
//}

std::string ControlUnit::getInfoFromUnit(Unit &unit) {
    std::string info = "";
    info += std::to_string(unit.getId()) + "-";
    info += unit.getActionState() + "-";
    info += std::to_string(unit.getCurrentPosition().getX()) + "-";
    info += std::to_string(unit.getCurrentPosition().getY()) + "-";
    info += std::to_string(unit.getLifeLeft()) + "|";
    return info;
}

std::string ControlUnit::getInfoFromOccupant(Occupant& Occupant) {
    std::string info = "";
    info += std::to_string(Occupant.getId()) + "-";
    info += std::to_string(Occupant.getPosition().getX()) + "-";
    info += std::to_string(Occupant.getPosition().getY()) + "-";
    info += std::to_string(Occupant.getLifeLeft()) + "|";
    return info;
}

std::string ControlUnit::getInfoFromBullets(Bullet &bullet) {
    std::string info = BULLET;
    info +=  "-" + bullet.getType() + "-";
    info += std::to_string(bullet.getId()) + "-";
    info += std::to_string(bullet.getPosition().getX()) + "-";
    info += std::to_string(bullet.getPosition().getY()) + "|";
    return info;
}

std::string ControlUnit::getInfoFromTerritory(Territory &territory) {
    Position flag_pos = territory.getFlagPosition();
    std::string info = "flagOn-";
    info += std::to_string(flag_pos.getX()) + "-";
    info += std::to_string(flag_pos.getY()) + "-";
    info += territory.getTeam() + "|";
    return info;
}

void ControlUnit::moveAllBullets() {
    std::vector<Bullet*>::iterator it = all_bullets.begin();
    for (; it != all_bullets.end();) {
        (*it)->move();
        if ((*it)->doYouHaveToDisapear()) {
            it = all_bullets.erase(it);
        } else {
            if ((*it)->didHit())
                (*it)->disapear();
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
