#ifndef Z_TPGRUPAL_ADDUNIT_H
#define Z_TPGRUPAL_ADDUNIT_H

#include <vector>
#include <string>
#include "Command.h"
#include "../UnitsMonitor.h"

class AddUnit : public Command {
    UnitsMonitor &unitsMonitor;
    const std::vector<std::string>& players;
public:
    explicit AddUnit(UnitsMonitor &unitsMonitor,
                     const std::vector<std::string>& players);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_ADDUNIT_H
