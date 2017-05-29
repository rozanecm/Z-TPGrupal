#ifndef Z_TPGRUPAL_UPDATEPOSITION_H
#define Z_TPGRUPAL_UPDATEPOSITION_H

#include <string>
#include <vector>
#include "Command.h"
#include "../PlayersMonitor.h"

class UpdatePosition : public Command {
    PlayersMonitor& monitor;
public:
    explicit UpdatePosition(PlayersMonitor& monitor);
    void execute(const std::vector<std::string>& args);
};

#endif //Z_TPGRUPAL_UPDATEPOSITION_H
