//
// Created by rodian on 27/05/17.
//

#ifndef Z_TPGRUPAL_COMMAND_H
#define Z_TPGRUPAL_COMMAND_H

#include "controUnit.h"

class Command {
private:
    std::string cmd;
    ControUnit* control;
    int unit_id, other_id, x,y;
public:
    Command(std::string& cmd, ControUnit* control);

    void run();

    void operator()();

private:
    std::string getNextData(std::string& line);
};


#endif //Z_TPGRUPAL_COMMAND_H
