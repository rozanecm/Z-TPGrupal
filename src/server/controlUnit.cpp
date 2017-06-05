//
// Created by rodian on 22/05/17.
//

#include "controlUnit.h"
#define WAIT 0.5


ControlUnit::ControlUnit(std::vector<Messenger*>& new_players,
                         std::vector<Unit>& all_units,
                         std::vector<Occupant>& all_occupants) :
    all_units(all_units), all_occupants(all_occupants), players(new_players),
    winning(false) {
    this->changed_units = new std::vector<Unit>;
    this->changed_occupants = new std::vector<int>;
}

void ControlUnit::run() {
    while(!winning) {
        double t3(WAIT);

        auto t1 = std::chrono::high_resolution_clock::now();
        // execute commands
        executeCommands();

        // do stuff
        this->unitsMakeMicroAction();
        this->checkAllLivingOccupants();
        auto t2 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> time_span =
             std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

        sleepFor(t3 - time_span.count());
        //send update message
        this->sendUpdateMessage();
    }
    // send victory or defeated message
}

void ControlUnit::sleepFor(double msec) {
    std::this_thread::sleep_for(std::chrono::duration<double> (msec));
}

void ControlUnit::unitsMakeMicroAction() {
    for (auto x: all_units){
        Unit y = x;
        x.makeAction();
        if (differenceOnUnits(x,y))
            changed_units->push_back(x);
    }
}


void ControlUnit::checkAllLivingOccupants() {
    std::vector<Occupant>::iterator it = all_occupants.begin();
    for(;it != all_occupants.end();){
        if(!it->areYouAlive()) {
            //erase it from map
            it = all_occupants.erase(it);
            // if building put ruins
        } else {
            ++it;
        }
    }
}

void ControlUnit::cmdMoveUnit(int id, int x, int y) {
    bool found = false;
    Unit selected_unit = all_units.front();
    std::vector<Unit>::iterator it = all_units.begin();
    while (it != all_units.end() && !found) {
        if (it->getId() == id) {
            (*it).calculateRoadTo(x, y);
            found = true;
        }
        ++it;
    }
}

void ControlUnit::executeCommands() {
    std::vector<Command> commands_copy;
    m.lock();
    for (auto x: *commands) {
        commands_copy.push_back(x);
    }
    commands->empty();
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
    std::string  update_msg = "";
    for (auto z: *changed_units) {
        update_msg += getInfoFromUnit(z);
    }
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
