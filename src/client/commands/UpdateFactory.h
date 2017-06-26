#ifndef Z_TPGRUPAL_UPDATEFACTORY_H
#define Z_TPGRUPAL_UPDATEFACTORY_H


#include "Command.h"
#include "../BuildingsMonitor.h"
#include "../MapMonitor.h"

class UpdateFactory : public Command {
    BuildingsMonitor& buildings;
    MapMonitor& map;
public:
    UpdateFactory(BuildingsMonitor& buildings, MapMonitor& map);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_UPDATEFACTORY_H
