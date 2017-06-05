//
// Created by rodian on 27/05/17.
//

#ifndef Z_TPGRUPAL_COMMAND_H
#define Z_TPGRUPAL_COMMAND_H

#include "controlUnit.h"
class ControlUnit;

class Command {
protected:
    std::string player_id, cmd;
    ControlUnit* control;
    int unit_id, other_id, x,y;
public:
    Command(std::string id, std::string& cmd, ControlUnit* control);

     void run();

     void operator()();

private:
    std::string getNextData(std::string& line);
};


#endif //Z_TPGRUPAL_COMMAND_H
