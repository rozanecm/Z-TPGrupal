#ifndef Z_TPGRUPAL_LOADMAP_H
#define Z_TPGRUPAL_LOADMAP_H


#include "../MapMonitor.h"
#include "Command.h"
#include "../BuildingsMonitor.h"
#include <string>
#include <vector>

class LoadMap : public Command{
    MapMonitor &mapMonitor;
    BuildingsMonitor &buildingsMonitor;

public:
    explicit LoadMap(MapMonitor &mapMonitor,
                     BuildingsMonitor& buildingsMonitor);

    void execute(const std::vector<std::string>& args);
};


#endif //Z_TPGRUPAL_LOADMAP_H
