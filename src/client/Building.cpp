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
