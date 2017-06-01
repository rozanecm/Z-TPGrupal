#include "Unit.h"
#include <utility>

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
