#ifndef Z_TPGRUPAL_REMOVEUNIT_H
#define Z_TPGRUPAL_REMOVEUNIT_H

#include <string>
#include <vector>
#include "Command.h"
#include "../UnitsMonitor.h"

class RemoveUnit : public Command {
    UnitsMonitor &monitor;
public:
    explicit RemoveUnit(UnitsMonitor &monitor);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_REMOVEUNIT_H
