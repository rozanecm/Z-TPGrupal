//
// Created by rodian on 22/05/17.
//

#include "unitMold.h"

UnitMold::UnitMold(int tec_level, int life, int range, int width, int height,
                   int unit_speed, int fire_rate, int time, int creation_quantity,
                   std::string &type, std::string &weapon_type) :
tec_level(tec_level), life(life), range(range), width(width), height(height),
unit_speed(unit_speed), fire_rate(fire_rate), creation_time(time),
creation_quantity(creation_quantity), type(type),weapon_type(weapon_type) {}

Unit *UnitMold::createUnit(int id, Size u_size, Map& map,
                           Weapon &weapon) {
    Position u_pos = u_size.getPosition();
    Compass* compass = new Compass(map, u_size,id,unit_speed);
    // get closest valid position from fabric
    Position valid_pos = compass->getAValidPositionForDestiny(u_pos);
    u_size.moveTo(valid_pos.getX(),valid_pos.getY());
    int x_range = valid_pos.getX() - range;
    int y_range = valid_pos.getY() - range;
    int w_range = range * 2 + width;
    int h_range = range * 2 + height;
    Size unit_range(x_range,y_range,w_range,h_range);


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

int UnitMold::getCreationQuantity() const {
    return creation_quantity;
}

std::string UnitMold::getWeaponType() const {
    return this->weapon_type;
}

int UnitMold::getFireRate() const {
    return fire_rate;
}

int UnitMold::getLife() const {
    return life;
}
