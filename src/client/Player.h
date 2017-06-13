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

    /**
     * This methods check all the player's units to see if any of its units is
     * located within the area of selection. If so, the units' attribute
     * selected is set to TRUE.
     */
    void markAsSelectedInRange(bool &unitsSelected, gdouble xStartCoordinate,
                                   gdouble yStartCoordinate,
                                   gdouble xFinishCoordinate,
                                   gdouble yFinishCoordinate);

    std::vector<Unit>
    getUnitsToDraw(unsigned int minX, unsigned int maxX, unsigned int minY,
                   unsigned int maxY);
};


#endif //Z_TPGRUPAL_PLAYER_H
