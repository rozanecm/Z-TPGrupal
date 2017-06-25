#ifndef Z_TPGRUPAL_LOSER_H
#define Z_TPGRUPAL_LOSER_H


#include "Command.h"
#include "../windows/GameWindow.h"

class Loser : public Command {
    MapMonitor& map;
    GameWindow& window;
public:
    Loser(MapMonitor& map, GameWindow &window);
    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_LOSER_H
