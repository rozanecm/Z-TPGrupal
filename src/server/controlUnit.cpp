//
// Created by rodian on 22/05/17.
//

#include "controlUnit.h"
#define WAIT 0.2

ControlUnit::ControlUnit(std::vector<Messenger *> &new_players,
                         std::map<int, Unit *> &all_units,
                         std::vector<Occupant *> &occupants,
                         std::vector<Team> &teams, CommandMonitor &commands,
                         std::vector<Territory *>& territories) :
    all_units(all_units), territories(territories),
    all_occupants(occupants), players(new_players), commands(commands),
    winning(false), teams(teams) {
}

void ControlUnit::run() {
    objects_counter = (int)all_occupants.size();
    while(!winning) {
        std::chrono::duration<double> t3(WAIT);

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

        this->checkForWinner();

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = t3 - (t2 - t1);
        std::cout << "sleep: "<< time_span.count() <<std::endl;
        sleepFor(time_span);
        changed_units.clear();
        changed_occupants.clear();
        changed_factories.clear();
    }
    // send victory or defeated message
    this->sendFinnalMessage();
}

void ControlUnit::sleepFor(std::chrono::duration<double> msec) {
    std::this_thread::sleep_for((msec));
//    std::this_thread::sleep_for(std::chrono::duration<double> (msec));
}

void ControlUnit::unitsMakeMicroAction() {
    // erase units with life 0
    std::vector<int> units_id;
    for (auto& x: all_units) {
        Unit& unit = *x.second;
        if (unit.doYouNeedToDisappear()) {
            units_id.push_back(x.first);
        }
    }
    for (auto& id: units_id) {
//        delete (all_units[id]);
        all_units.erase(id);
    }

    // units alive make micro action
    for (auto& x: all_units){
        Unit& unit = *x.second;
        // check if someone changed the unit
        bool was_changed = false;
        if (unit.haveYouChanged()) {
            changed_units.push_back(unit.getId());
            was_changed = true;
        }
        unit.makeAction();
        // check if the unit changed
        if (unit.haveYouChanged() && !was_changed) {
            changed_units.push_back(unit.getId());
        }
        if (!unit.areYouAlive()) {
            unit.mustDisappear();
        } else if (unit.doYouHaveAnyBullets()) {
            std::vector<Bullet *> tmp = unit.collectBullets();
            for (auto& b: tmp) {
                b->setCorrectId(objects_counter);
                ++objects_counter;
            }
            all_bullets.insert(all_bullets.end(), tmp.begin(), tmp.end());
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

void ControlUnit::makeTerritoriesChecks() {
    for (auto& t: territories) {
        if (t->doesTerritorysOwnerChanged()) {
            std::string info = "updateterritory-";
            info += std::to_string(t->getId()) + t->getTeam();
            Position flag = t->getFlagPosition();
            info += std::to_string(flag.getX()) + "-" +
                    std::to_string(flag.getY());
            for (auto& team: teams) {
                std::vector<PlayerInfo>& players = team.getPlayersInfo();
                for (auto& p : players) {
                    // the last owner must eliminate the territory from his
                    // vector
                    p.eliminateThisTerritory(t);

                    // the new owner must add it
                    if (t->getTeam() == p.getPlayerId()) {
                        p.addTerritory(t);
                    }
                }
            }
            for (auto y: players) {
                y->sendMessage(info);
            }
        }
    }
    makeFactoryChecks();
}

void ControlUnit::makeFactoryChecks() {
    for (auto t: territories) {
        std::map<int,Factory*>& factories = t->getFactories();
        auto it = factories.begin();
        // vector to know witch factories erase
        std::vector<int> factories_id;
        for (; it != factories.end();) {
            Factory *f = it->second;
            if (f->areYouAlive()) {
                bool was_changed = false;
                if (f->haveYouChanged()) {
                    changed_factories.push_back(*f);
                    was_changed = true;
                }
                f->build(objects_counter);
                // check if the factory changed
                if (f->haveYouChanged() && !was_changed) {
                    changed_factories.push_back(*f);
                }
                if (f->haveNewUnits()) {
                    std::vector<Unit *> tmp = f->getUnits();
                    std::string msg = "";
                    for (auto &u: tmp) {
                        u->recalculateMyStartPosition();
                        all_units[u->getId()] = u;
                        all_occupants.push_back((Occupant*)u);
                        // set changed boolean to false
                        u->haveYouChanged();
                        msg += "addunit-";
                        msg += getInfoForAddUnit(*u);
                    }
                    for (auto y: players) {
                        y->sendMessage(msg);
                    }
                }
            }
            ++it;
        }
        for (auto& fact: factories_id) {
            factories.erase(fact);
        }
    }
}

void ControlUnit::cmdMoveUnit(const std::string& id_player,int id, int x,
                              int y) {
    std::map<int,Unit*>::iterator it;
    it = all_units.find(id);
    if ((*it->second).getTeam() == id_player)
        (*it->second).calculateRoadTo(x,y);
}

void ControlUnit::cmdAttack(const std::string& attacker_team, int id_unit,
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

void ControlUnit::cmdGrab(const std::string &id_player, int id_unit,
                          int target) {
    std::map<int,Unit*>::iterator it;
    it = all_units.find(id_unit);
    Unit& unit = (*it->second);
    bool found = false;
    if (unit.getTeam() == id_player) {
        // for (auto t: territories) {
//        if (flag.id == target)
//    }
        if (!found) {
            for (auto& z: all_occupants) {
                if (z->getId() == target) {
                    unit.setTargetToGrab(z, z->getType());
                }
            }
        }
    }
}

void ControlUnit::cmdFactoryCreate(const std::string& player_id,
                                   int id_factory) {
    for (auto t: territories) {
        std::map<int, Factory *> &factories = t->getFactories();
        for (auto& f: factories) {
            if (f.first == id_factory && f.second->areYouAlive()) {
                f.second->startBuilding(player_id);
            }
        }
    }
}

void ControlUnit::cmdFactoryNext(const std::string &player_id, int id_factory) {
    std::string info = "";
    for (auto t: territories) {
        std::map<int, Factory *> &factories = t->getFactories();
        for (auto& f: factories) {
            if (f.first == id_factory && f.second->getTeam() == player_id
                                         && f.second->areYouAlive()) {
                UnitMold* mold = f.second->nextUnit();
                info += "factorystats-";
                int creation_time = f.second->getCreationSpeed();
                info += getInfoFromUnitMold(*mold,creation_time);
                break;
            }
        }
    }
    sendMessageTo(player_id,info);
}

void ControlUnit::cmdFactoryPrev(const std::string &player_id, int id_factory) {
    std::string info = "";
    for (auto t: territories) {
        std::map<int, Factory *> &factories = t->getFactories();
        for (auto& f: factories) {
            if (f.first == id_factory && f.second->getTeam() == player_id
                                         && f.second->areYouAlive()) {
                UnitMold* mold = f.second->previousUnit();
                info += "factorystats-";
                int creation_time = f.second->getCreationSpeed();
                info += getInfoFromUnitMold(*mold,creation_time);
                break;
            }
        }
    }
    sendMessageTo(player_id,info);
}


void ControlUnit::cmdFactoryCurrent(const std::string &player_id,
                                    int id_factory) {
    std::string info = "";
    for (auto t: territories) {
        std::map<int, Factory *> &factories = t->getFactories();
        for (auto& f: factories) {
            if (f.first == id_factory && f.second->getTeam() == player_id
                                         && f.second->areYouAlive()) {
                UnitMold* mold = f.second->getSelectedUnit();
                info += "factorystats-";
                int creation_time = f.second->getCreationSpeed();
                info += getInfoFromUnitMold(*mold,creation_time);
                break;
            }
        }
    }
    sendMessageTo(player_id,info);
}


void ControlUnit::executeCommands() {
    std::vector<Command> commands_copy;
    commands.copyCommands(commands_copy);

    // Execute command
    for (auto cmd: commands_copy) {
        cmd();
    }
}

void ControlUnit::sendMessageTo(const std::string& player_id,
                                std::string& msg) {
    bool found = false;
    for (auto& t: teams) {
        std::vector<PlayerInfo>& plyrs = t.getPlayersInfo();
        for (auto& p: plyrs) {
            if (p.getPlayerId() == player_id) {
                p.getMessenger()->sendMessage(msg);
                found = true;
                break;
            }
        }
        if (found)
            break;
    }
}

void ControlUnit::sendUpdateMessage() {
    std::string info = getUpdateInfo();
    if (!info.size()) {
        return;
    }
    for (auto y: players) {
        y->sendMessage(info);
    }
}

std::string ControlUnit::getUpdateInfo() {
    std::string  update_msg = "";
    for (auto z: changed_units) {
        update_msg += "updateunit-";
        update_msg += getInfoFromUnit(*all_units.at(z));
    }

    for (auto y: changed_occupants) {
        update_msg += "updateoccupant-";
        update_msg += getInfoFromOccupant(y);
    }

    for (auto& f: changed_factories) {
        update_msg += "updatefactory-";
        update_msg += getInfoFromFactories(f);
    }

//    for (auto b: all_bullets) {
//        update_msg += "updatebullet-";
//        update_msg += getInfoFromBullets(*b) ;
//    }

//    for(auto t: territories) {
//       if (t.doesTerritorysOwnerChanged()) {
//           update_msg += getInfoFromTerritory(t);
//       }
//    }

    return update_msg;
}

std::string ControlUnit::getInfoFromUnit(Unit &unit) {
    std::string info = "";
    info += std::to_string(unit.getId()) + "-";
    info += unit.getActionState() + "-";
    info += std::to_string(unit.getCurrentPosition().getX()) + "-";
    info += std::to_string(unit.getCurrentPosition().getY()) + "-";
    info += std::to_string(unit.getLifeLeft()) + "-";
    info += unit.getTeam() + "|";
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

std::string ControlUnit::getInfoFromFactories(Factory &factory) {
    std::string info = "";
    info += std::to_string(factory.getId()) + "-";
//    info += factory.getSelectedUnit() + "-";
    // This is the time needed before the next unit is build in seconds
    double time = WAIT * factory.getCreationSpeed();
    int min = 0, sec = 0;
    getTime(min, sec, time);
    info += std::to_string(min)+ "-" +std::to_string(sec) + "-";
    info += std::to_string(factory.getLifeLeft()) + "-";
    info += factory.getTeam() + "|";
    return info;
}

std::string ControlUnit::getInfoFromBullets(Bullet &bullet) {
    std::string info = "";
    info +=  bullet.getType() + "-";
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

std::string ControlUnit::getInfoFromUnitMold(UnitMold &mold,
                                             double creation_time) {
    std::string info = "";
    info += mold.getTypeOfUnit() + "-";
    info += std::to_string(mold.getFireRate()) + "-";
    double time = WAIT * creation_time;
    int min = 0, sec = 0;
    getTime(min, sec, time);
    info += std::to_string(min)+ "-" +std::to_string(sec) + "-";
    info += std::to_string(mold.getLife()) + "|";
    return info;
}

std::string ControlUnit::getInfoForAddUnit(Unit &unit) {
    std::string info = "";
    info += std::to_string(unit.getId()) + "-";
    Position pos = unit.getPosition();
    info += std::to_string(pos.getX()) + "-";
    info += std::to_string(pos.getY()) + "-";
    info += unit.getType() + "-";
    info += unit.getTeam() + "-";
    info += std::to_string(unit.getLifeLeft()) + "|";
    return info;
}

void ControlUnit::moveAllBullets() {
    std::vector<Bullet*>::iterator it = all_bullets.begin();
    for (; it != all_bullets.end();) {
        (*it)->move();
        if ((*it)->doYouHaveToDisapear()) {
            delete((*it));
            it = all_bullets.erase(it);
        } else {
            if ((*it)->didHit())
                (*it)->disapear();
            ++it;
        }
    }
}


void ControlUnit::checkForWinner() {
    int teams_alive = 0;
    for (auto t: teams) {
        if (!t.doesTeamLose()) {
            teams_alive += 1;
        } else {
            std::vector<PlayerInfo>& losers = t.getPlayersInfo();
            for (auto& w: losers) {
                w.getMessenger()->sendMessage("loseryousuck");
            }
        }
    }

    if (teams_alive == 1) {
        winning = true;
    }
}

void ControlUnit::sendFinnalMessage() {
    std::string winner = "winner-";
    for (auto& t: teams) {
        if (!t.doesTeamLose()) {
            std::vector<PlayerInfo>& winners = t.getPlayersInfo();
            for (auto& w: winners) {
                w.getMessenger()->sendMessage("winner");
            }
        } else {
            std::vector<PlayerInfo>& losers = t.getPlayersInfo();
            for (auto& w: losers) {
                w.getMessenger()->sendMessage("loseryousuck");
            }
        }
    }
}

void ControlUnit::getTime(int &minutes, int &seconds, double time) {
    double min = time/60;
    minutes = (int)min;
    double sec = min - minutes;
    sec = sec * 60;
    seconds = (int) sec;
}


