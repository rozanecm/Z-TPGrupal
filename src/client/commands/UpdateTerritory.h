#ifndef Z_TPGRUPAL_UPDATETERRITORY_H
#define Z_TPGRUPAL_UPDATETERRITORY_H


#include "Command.h"
#include "../MapMonitor.h"

class UpdateTerritory : public Command{
    MapMonitor& mapMonitor;
public:
    UpdateTerritory(MapMonitor& mapMonitor1);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_UPDATETERRITORY_H
