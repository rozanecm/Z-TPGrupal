#ifndef Z_TPGRUPAL_ADDUNIT_H
#define Z_TPGRUPAL_ADDUNIT_H

#include <vector>
#include <string>
#include "Command.h"
#include "../PlayersMonitor.h"

class AddUnit : public Command {
    PlayersMonitor& playerMonitor;
public:
    explicit AddUnit(PlayersMonitor& monitor);
    void execute(const std::vector<std::string>& args);
};


#endif //Z_TPGRUPAL_ADDUNIT_H
