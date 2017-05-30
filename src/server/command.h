//
// Created by rodian on 27/05/17.
//

#ifndef Z_TPGRUPAL_COMMAND_H
#define Z_TPGRUPAL_COMMAND_H

#include "controlUnit.h"
class ControlUnit;

class Command {
private:
    std::string cmd;
    int unit_id, other_id, x,y;
public:
    Command(std::string& cmd);

    virtual void run() = 0;

    virtual void operator()() = 0;

private:
    std::string getNextData(std::string& line);
};


#endif //Z_TPGRUPAL_COMMAND_H
