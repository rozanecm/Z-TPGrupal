#ifndef Z_TPGRUPAL_UPDATEPOSITION_H
#define Z_TPGRUPAL_UPDATEPOSITION_H

#include <string>
#include <vector>
#include "Command.h"
#include "../UnitsMonitor.h"
#include "../MapMonitor.h"
#include "../ServerMessenger.h"

class UpdatePosition : public Command {
    UnitsMonitor &units;
public:
    explicit UpdatePosition(UnitsMonitor &units);

    void execute(const std::vector<std::string> &args);
};

#endif //Z_TPGRUPAL_UPDATEPOSITION_H
