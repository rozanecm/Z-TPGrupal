#include "Unit.h"
#include "enums/UnitsEnum.h"
#include "enums/RotationsEnum.h"
#include <utility>
#include <Lock.h>

int Unit::get_ID() {
    return id;
}

void Unit::update_position(int x, int y) {
    std::pair<int, int> new_pos(x, y);

    prev_position = position;
    position = new_pos;
}

void
Unit::markAsSelectedInRange(gdouble xStartCoordinate,
                            gdouble yStartCoordinate,
                            gdouble xFinishCoordinate,
                            gdouble yFinishCoordinate) {
    if (position.first >= xStartCoordinate &&
            position.first <= xFinishCoordinate &&
            position.second >= yStartCoordinate &&
            position.second <= yFinishCoordinate){
        selected = true;
    }
}

bool Unit::isShooting() {
    return shooting;
}

TeamEnum Unit::getTeam() {
    return team;
}

RotationsEnum Unit::getRotation() {
    return rotation;
}

unsigned int Unit::getXCoordinate() {
    return position.first;
}

unsigned int Unit::getYCoordinate() {
    return position.second;
}

UnitsEnum Unit::getType() {
    return unitType;
}

ActionsEnum Unit::getAction() {
    /* since vehicles don't have moving animations... */
    //todo possible refactor. Check this outside (just one option listed here)
    if (unitType == UnitsEnum::HEAVY_TANK or
            unitType == UnitsEnum::JEEP or
            unitType == UnitsEnum::MML or
            unitType == UnitsEnum::LIGHT_TANK or
            unitType == UnitsEnum::MEDIUM_TANK){
        return ActionsEnum::STAND;
    }

    if (shooting) {
        return ActionsEnum::FIRE;
    } else if (position == prev_position) {
        return ActionsEnum::STAND;
    } else {
        return ActionsEnum::MOVE;
    }
}
