#ifndef Z_TPGRUPAL_PLAYER_H
#define Z_TPGRUPAL_PLAYER_H


#include <vector>
#include "Unit.h"

class Player {
    int id;
    /* Units; robots and vehicles */
    std::vector<Unit> units;
public:
    int getID();
    void addUnit(Unit& unit);
    bool removeUnit(int id);
    bool update_position(int id, int x, int y);
};


#endif //Z_TPGRUPAL_PLAYER_H
