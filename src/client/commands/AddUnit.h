#ifndef Z_TPGRUPAL_ADDUNIT_H
#define Z_TPGRUPAL_ADDUNIT_H

#include <vector>
#include <string>
#include "Command.h"
#include "../UnitsMonitor.h"
#include "../MapMonitor.h"

class AddUnit : public Command {
    UnitsMonitor &unitsMonitor;
    MapMonitor& map;
public:
    explicit AddUnit(UnitsMonitor &unitsMonitor,
                     MapMonitor& map);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_ADDUNIT_H
