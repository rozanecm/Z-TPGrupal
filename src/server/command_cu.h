//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_COMMAND_CU_H
#define Z_TPGRUPAL_COMMAND_CU_H


#include "command.h"

class CommandCU: public Command {
private:
    std::string cmd;
    ControlUnit* control;
    int unit_id, other_id, x,y;
public:
    CommandCU(std::string& cmd, ControlUnit* control);

    void run();

    void operator()();

private:
    std::string getNextData(std::string& line);
};


#endif //Z_TPGRUPAL_COMMAND_CU_H
