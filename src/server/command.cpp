//
// Created by rodian on 27/05/17.
//

#include "command.h"

Command::Command(std::string& cmd) : cmd(cmd) {}


std::string Command::getNextData(std::string& line) {
    std::size_t found = line.find('-');
    std::string data = line.substr(0,found);
    line.erase(0,found+1);
    return data;
}