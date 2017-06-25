#ifndef Z_TPGRUPAL_ADDBUILDING_H
#define Z_TPGRUPAL_ADDBUILDING_H


#include "../BuildingsMonitor.h"
#include "Command.h"
#include "../MapMonitor.h"

class AddBuilding : public Command {
    BuildingsMonitor &buildings;
    MapMonitor& map;
public:
    explicit AddBuilding(BuildingsMonitor &buildings,
                         MapMonitor& map);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_ADDBUILDING_H
