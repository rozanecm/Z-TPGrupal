//
// Created by rodian on 27/05/17.
//

#include "command.h"

Command::Command(std::string id,std::string& cmd, ControlUnit* control) :
        player_id(id), cmd(cmd), control(control) {}

void Command::run() {
    std::string action = getNextData(cmd);
    // if command is to move
    if (action == "mv") {
        // get id, then position (x,y)
        std::string id_str = getNextData(cmd);
        int id = std::stoi(id_str);
        std::string x_str = getNextData(cmd);
        int x = std::stoi(x_str);
        std::string y_str = getNextData(cmd);
        int y = std::stoi(y_str);
        // make move
        control->cmdMoveUnit(player_id,id,x,y);
    } else if (action == "atk") {
        std::string id_unit_str = getNextData(cmd);
        int id_unit = std::stoi(id_unit_str);
        std::string target_str = getNextData(cmd);
        int target = std::stoi(target_str);
        control->cmdAttack(player_id,id_unit,target);
    } else if (action == "grab") {
    }
}

void Command::operator()() {
    this->run();
}

std::string Command::getNextData(std::string& line) {
    std::size_t found = line.find('-');
    std::string data = line.substr(0,found);
    line.erase(0,found+1);
    return data;
}