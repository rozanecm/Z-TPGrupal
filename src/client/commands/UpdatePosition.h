#ifndef Z_TPGRUPAL_UPDATEPOSITION_H
#define Z_TPGRUPAL_UPDATEPOSITION_H

#include <string>
#include <vector>
#include "Command.h"
#include "../UnitsMonitor.h"

class UpdatePosition : public Command {
    UnitsMonitor &monitor;
public:
    explicit UpdatePosition(UnitsMonitor &monitor);

    void execute(const std::vector<std::string> &args);
};

#endif //Z_TPGRUPAL_UPDATEPOSITION_H
