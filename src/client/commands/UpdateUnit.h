#ifndef Z_TPGRUPAL_UPDATE_H
#define Z_TPGRUPAL_UPDATE_H


#include "Command.h"
#include "../UnitsMonitor.h"
#include "../MapMonitor.h"
#include "../ServerMessenger.h"

class UpdateUnit : public Command {
    UnitsMonitor& units;
    MapMonitor& map;
    ServerMessenger& messenger;
public:
    explicit UpdateUnit(UnitsMonitor &units, MapMonitor& map,
                        ServerMessenger& messenger);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_UPDATE_H
