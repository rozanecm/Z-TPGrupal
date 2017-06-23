#ifndef Z_TPGRUPAL_UPDATE_H
#define Z_TPGRUPAL_UPDATE_H


#include "Command.h"
#include "../UnitsMonitor.h"

class UpdateUnit : public Command {
    UnitsMonitor& units;
public:
    explicit UpdateUnit(UnitsMonitor& units);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_UPDATE_H
