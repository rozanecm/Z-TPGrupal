#include "Building.h"

#define LENIENCY 8
void Building::markAsSelectedInRange(bool &buildingSelected,
                                     gdouble xStartCoordinate,
                                     gdouble yStartCoordinate,
                                     gdouble xFinishCoordinate,
                                     gdouble yFinishCoordinate) {
    int x = (int) xFinishCoordinate;
    int y = (int) yFinishCoordinate;
    int x_abs = abs(position.first - x);
    int y_abs = abs(position.second - y);

    if (x_abs <= LENIENCY && y_abs <= LENIENCY) {
        selected = true;
        buildingSelected = true;
    }
}

Building::Building(BuildingsEnum type, int x, int y, int id, TeamEnum team) :
        buildingType(type),
        position(x, y),
        id(id),
        selected(false),
        team (team) {
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

TeamEnum Building::getTeam() {
    return team;
}

bool Building::is_selected() {
    return selected;
}

void Building::unselect() {
    selected = false;
}
