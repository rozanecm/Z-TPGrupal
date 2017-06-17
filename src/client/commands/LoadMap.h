#ifndef Z_TPGRUPAL_LOADMAP_H
#define Z_TPGRUPAL_LOADMAP_H


#include "../MapMonitor.h"
#include "Command.h"
#include "../BuildingsMonitor.h"
#include "../windows/GameWindow.h"
#include <string>
#include <vector>

class LoadMap : public Command{
    MapMonitor &mapMonitor;
    BuildingsMonitor &buildingsMonitor;
    GameWindow &window;

public:
    LoadMap(MapMonitor &monitor, BuildingsMonitor &buildingsMonitor,
            GameWindow &window);

    void execute(const std::vector<std::string>& args);
};


#endif //Z_TPGRUPAL_LOADMAP_H
