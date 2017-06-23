#ifndef Z_TPGRUPAL_UPDATEFACTORY_H
#define Z_TPGRUPAL_UPDATEFACTORY_H


#include "Command.h"
#include "../BuildingsMonitor.h"

class UpdateFactory : public Command {
    BuildingsMonitor& buildings;
public:
    UpdateFactory(BuildingsMonitor& buildings);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_UPDATEFACTORY_H
