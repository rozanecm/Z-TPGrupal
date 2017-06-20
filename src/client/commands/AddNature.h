#ifndef Z_TPGRUPAL_ADDNATURE_H
#define Z_TPGRUPAL_ADDNATURE_H


#include "Command.h"
#include "../MapMonitor.h"

class AddNature : public Command {
    MapMonitor &mapMonitor;
public:
    explicit AddNature(MapMonitor &mapMonitor);

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_ADDNATURE_H
