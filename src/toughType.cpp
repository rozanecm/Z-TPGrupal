//
// Created by rodian on 21/05/17.
//

#include "toughType.h"

ToughType::ToughType(int tec_level, int life, int range, int width, int height,
                     int unit_speed, int fire_rate, int time) :
tec_level(tec_level), life(life), range(range), width(width), height(height),
unit_speed(unit_speed), fire_rate(fire_rate), creation_time(time) {}

Unit *ToughType::createTought(int x, int y, Map& map) {
    Size unit_size(x,y,width,height);
    Size unit_range(x,y,range,range);
    Unit* new_tough = new Unit(life,"Tough", 4,unit_size,unit_range,map);
    return new_tough;
}

int ToughType::getTechnologyLevel() const {
    return tec_level;
}

int ToughType::getCreationTime() {
    return creation_time;
}
