#ifndef Z_TPGRUPAL_BUILDING_H
#define Z_TPGRUPAL_BUILDING_H

#include <gtkmm/drawingarea.h>
#include "Player.h"
#include "enums/BuildingsEnum.h"
#include <utility>
#include <map>
#include <string>
#include <vector>

class Building {
private:
    BuildingsEnum buildingType;
    std::pair<int, int> position;

    /* bool selected: indicates wether the unit has been selected
     * with the mouse or not */
    bool selected;
    int id;
public:
    Building(BuildingsEnum type, int x, int y, int id);
    /**
     * This methods checks all the player's units to see if any of its units is
     * located within the area of selection. If so, the units' attribute
     * selected is set to TRUE.
     */
    void markAsSelectedInRange(gdouble xStartCoordinate,
                               gdouble yStartCoordinate,
                               gdouble xFinishCoordinate,
                               gdouble yFinishCoordinate);
};


#endif //Z_TPGRUPAL_BUILDING_H
