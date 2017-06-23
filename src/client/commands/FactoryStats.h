#ifndef Z_TPGRUPAL_FACTORYSTATS_H
#define Z_TPGRUPAL_FACTORYSTATS_H


#include "Command.h"
#include "../BuildingsMonitor.h"

class FactoryStats : public Command {
    BuildingsMonitor& buildings;
public:
    FactoryStats(BuildingsMonitor& buildings);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_FACTORYSTATS_H
