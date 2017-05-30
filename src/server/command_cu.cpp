//
// Created by rodian on 29/05/17.
//

#include "command_cu.h"

CommandCU::CommandCU(std::string& cmd, ControlUnit* control) : Command(cmd),
                                                           control(control) {}

void CommandCU::run() {
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
        control->cmdMoveUnit(id,x,y);
    } else if (action == "atck") {
    } else if (action == "grab") {
    }
}

void CommandCU::operator()() {
    this->run();
}

std::string CommandCU::getNextData(std::string& line) {
    std::size_t found = line.find('-');
    std::string data = line.substr(0,found);
    line.erase(0,found+1);
    return data;
}