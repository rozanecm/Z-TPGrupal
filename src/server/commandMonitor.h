//
// Created by rodian on 15/06/17.
//

#ifndef Z_TPGRUPAL_COMMANDMONITOR_H
#define Z_TPGRUPAL_COMMANDMONITOR_H


#include "command.h"
class Command;
class ControlUnit;

class CommandMonitor {
private:
    std::vector<Command> commands;
    std::mutex& m;
public:
    CommandMonitor(std::mutex& m);

    void addCommand(std::string id, std::string& cmd, ControlUnit* control);

    void copyCommands(std::vector<Command>& commands_copy);
};


#endif //Z_TPGRUPAL_COMMANDMONITOR_H
