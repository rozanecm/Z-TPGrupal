//
// Created by rodian on 15/06/17.
//

#include "commandMonitor.h"

CommandMonitor::CommandMonitor(std::mutex &m) : m(m) {}

void CommandMonitor::addCommand(std::string id, std::string& cmd,
                                ControlUnit* control) {
    Lock l(m);
    commands.push_back(Command(id, cmd, control));
}

void CommandMonitor::copyCommands(std::vector<Command> &commands_copy) {
    Lock l(m);
    for (auto cmd: commands) {
        commands_copy.push_back(cmd);
    }

    commands.clear();
}
