#ifndef Z_TPGRUPAL_PLAYERMONITOR_H
#define Z_TPGRUPAL_PLAYERMONITOR_H


#include <vector>
#include <mutex>
#include "Player.h"

class PlayersMonitor {
    std::vector<Player> &players;

    std::mutex m;
public:
    PlayersMonitor(std::vector<Player> &players);
};


#endif //Z_TPGRUPAL_PLAYERMONITOR_H
