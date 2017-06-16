#include "Unit.h"
#include "enums/UnitsEnum.h"
#include "enums/RotationsEnum.h"
#include <utility>
#include <Lock.h>
#include <iostream>

#define IMG_SIZE_IN_PX 16

Unit::Unit(int id, std::pair<unsigned int, unsigned int> position,
           UnitsEnum unitType, RotationsEnum rotation)
        : id(id), position(position), unitType(unitType){
    //todo check what else should be initialized
}

int Unit::get_ID() {
    return id;
}

void Unit::update_position(int x, int y) {
    std::pair<int, int> new_pos(x, y);

    prev_position = position;
    position = new_pos;
}

void
Unit::markAsSelectedInRange(bool &unitsSelected, gdouble xStartCoordinate,
                            gdouble yStartCoordinate,
                            gdouble xFinishCoordinate,
                            gdouble yFinishCoordinate) {
    if (position.first >= xStartCoordinate - IMG_SIZE_IN_PX &&
            position.first <= xFinishCoordinate + IMG_SIZE_IN_PX &&
            position.second >= yStartCoordinate - IMG_SIZE_IN_PX &&
            position.second <= yFinishCoordinate + IMG_SIZE_IN_PX){
        selected = true;
        unitsSelected = true;
        //todo debug cerr
        std::cerr<<"unit selected"<<std::endl;
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

bool Unit::is_selected() {
    return selected;
}
