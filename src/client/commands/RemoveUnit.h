#ifndef Z_TPGRUPAL_REMOVEUNIT_H
#define Z_TPGRUPAL_REMOVEUNIT_H

#include <string>
#include <vector>
#include "Command.h"
#include "../PlayersMonitor.h"

class RemoveUnit : public Command {
    PlayersMonitor& monitor;
public:
    explicit RemoveUnit(PlayersMonitor& monitor);

    void execute(const std::vector<std::string>& args);
};


#endif //Z_TPGRUPAL_REMOVEUNIT_H
