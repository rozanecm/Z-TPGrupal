#ifndef Z_TPGRUPAL_FACTORYSTATS_H
#define Z_TPGRUPAL_FACTORYSTATS_H


#include "Command.h"
#include "../BuildingsMonitor.h"
#include "../windows/GameWindow.h"

class FactoryStats : public Command {
    GameWindow& window;
public:
    FactoryStats(GameWindow& window);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_FACTORYSTATS_H
