#include "Building.h"

void Building::markAsSelectedInRange(gdouble xStartCoordinate,
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

Building::Building(BuildingsEnum type, int x, int y) :
    buildingType(type),
    position(x, y),
    selected(false)
{
}

unsigned int Building::getXCoordinate() {
    return position.first;
}

unsigned int Building::getYCoordinate() {
    return position.second;
}

BuildingsEnum Building::getBuildingType() {
    return buildingType;
}
