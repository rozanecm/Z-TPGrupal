#ifndef Z_TPGRUPAL_PLAYERMONITOR_H
#define Z_TPGRUPAL_PLAYERMONITOR_H


#include <vector>
#include <mutex>
#include "Player.h"

class PlayersMonitor {
    std::vector<Player> &players;
    std::mutex m;
public:
    explicit PlayersMonitor(std::vector<Player> &players);

    void addUnit(int player_id, Unit& unit);
    void removeUnit(int unit_id);
    void update_position(int unit_id, int x, int y);
};


#endif //Z_TPGRUPAL_PLAYERMONITOR_H
