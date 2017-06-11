//
// Created by rodian on 22/05/17.
//

#include "unitMold.h"

UnitMold::UnitMold(int tec_level, int life, int range, int width, int height,
                     int unit_speed, int fire_rate, int time, std::string type):
tec_level(tec_level), life(life), range(range), width(width), height(height),
unit_speed(unit_speed), fire_rate(fire_rate), creation_time(time), type(type) {}

Unit *UnitMold::createUnit(int id, Size u_size, Map& map,
                           Weapon &weapon) {
    Position u_pos = u_size.getPosition();
    Size unit_range(u_pos.getX(),u_pos.getY(),range,range);
    Compass compass(map,u_size,unit_speed);
    Unit* new_unit = new Unit(id, life, type, unit_speed, u_size, unit_range,
                              compass, weapon, fire_rate);
    return new_unit;
}

int UnitMold::getTechnologyLevel() const {
    return tec_level;
}

int UnitMold::getCreationTime() {
    return creation_time;
}

std::string UnitMold::getTypeOfUnit() const {
    return type;
}

Size UnitMold::getUnitSize() {
    return Size(0, 0, width, height);
}
