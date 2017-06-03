#include "Unit.h"

int Unit::get_ID() {
    return id;
}

void Unit::update_position(int x, int y) {
    std::pair<int, int> new_pos(x, y);

    prev_position = position;
    position = new_pos;
}
