#ifndef Z_TPGRUPAL_ADDBUILDING_H
#define Z_TPGRUPAL_ADDBUILDING_H


#include "../BuildingsMonitor.h"
#include "Command.h"

class AddBuilding : public Command {
    BuildingsMonitor &buildings;
public:
    explicit AddBuilding(BuildingsMonitor &buildings);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_ADDBUILDING_H
