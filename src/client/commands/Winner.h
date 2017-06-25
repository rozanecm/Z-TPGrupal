#ifndef Z_TPGRUPAL_WINNER_H
#define Z_TPGRUPAL_WINNER_H


#include "Command.h"
#include "../windows/GameWindow.h"
#include "../MapMonitor.h"

class Winner : public Command {
    MapMonitor& map;
    GameWindow& window;

public:
    Winner(MapMonitor& map, GameWindow& window);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_WINNER_H
