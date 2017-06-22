//
// Created by rodian on 18/05/17.
//

#include "unit.h"
#define NEUTRAL "Neutral"
Unit::Unit(int id, int life, std::string type, int unit_speed, Size size,
           Size range, Compass &compass, Weapon &weapon, int fire_rate) :
        Occupant(id, life, type, size), compass(compass), weapon(weapon),
        unit_speed(unit_speed),fire_rate(fire_rate),fire_count(0),
        state(STANDINGSTATE),action(STANDINGSTATE), range(range), target(this)
        ,grab_target(this),got_target(false),mount_vehicule(false){
    compass.changeUnitId(id);
}

void Unit::makeAction() {
    if (this->state == STANDINGSTATE) {
        if (this->team != NEUTRAL) {
            if (!got_target) {
                // Check for enemies around you. If so, state = ATKSTATE
                this->changed = false;
                target = compass.checkForEnemiesOnRange
                                                    (*(Occupant *) this, range);
                if (target->getId() != this->id) {
                    got_target = true;
                }
            } else {
                if (target->areYouAlive() && checkIfTargetIsOnRange()) {
                    attack();
                    this->action = ATKSTATE;
                } else {
                    got_target = false;
                    this->action = STANDINGSTATE;
                    this->changed = true;
                }
            }
        }
    }
    if (this->state == MOVESTATE) {
        this->move();
        if (road.empty()) {
            this->state = STANDINGSTATE;
            this->action = STANDINGSTATE;
            this->changed = true;
        }
    }
    if (this->state == ATKSTATE) {
        if (target->areYouAlive()) {
            if (checkIfTargetIsOnRange()) {
                if (!road.empty())
                    road.clear();
                attack();
            } else {
                // If target is not on range move till it is
                // calculate road to target
                if (road.empty()) {
                    Position trg_pos = target->getPosition();
                    getOnRangeOf(trg_pos.getX(), trg_pos.getY());
                }
                move();
            }
        } else {
            this->state = STANDINGSTATE;
            this->action = STANDINGSTATE;
            this->changed = true;
        }
    }
    if (this->state == GRABBINGSTATE) {
        if (this->action != MOVESTATE) {
            Position target_pos = grab_target->getPosition();
            if (!road.empty())
                road.clear();
            Position actual = obj_size.getPosition();
            road = compass.getFastestWay(actual, target_pos);
            this->action = MOVESTATE;
        } else {
            if (onRangeToGrabTarget()) {
                grab();
            } else if (road.empty()) {
                this->state = STANDINGSTATE;
                this->action = STANDINGSTATE;
                this->changed = true;
                mount_vehicule = true;
            } else if (!road.empty()) {
                move();
            }
        }
    }
}

void Unit::calculateRoadTo(int x, int y) {
    if (!checkIfAlreadyOnMyWay(x,y)) {
        this->state = MOVESTATE;
        Position destination(x, y);
        Position actual = obj_size.getPosition();
        road = compass.getFastestWay(actual, destination);
    }
}

void Unit::getOnRangeOf(int x, int y) {
    Position destination(x,y);
    Position actual = obj_size.getPosition();
    road = compass.getFastestWay(actual,destination);
}

void Unit::move() {
    int distance = unit_speed;
    int steps = 0;
    bool crash = false;
    compass.clearCompass();
//    if (!road.empty()) {
//        Position actual = obj_size.getPosition();
//        double t_factor = compass.getTerrainFactorOn(
//                                                 actual.getX(),actual.getY());
//        // increase or decrease distance til steps are more than unit speed
//        if (unit_speed == 4) {
//            distance = (int) (t_factor * unit_speed);
//        } else if (unit_speed > 4) {
//            distance = (int) (t_factor * unit_speed *
//                              (1-(damage_recv/life_points)));
//        }
//        Size next_pos = getNextPosition(distance);
//        Position pos = next_pos.getPosition();
//        if (compass.canIWalkToThisPosition(next_pos)) {
//            // move unit position, range and weapon
//            this->obj_size.moveTo(pos.getX(),pos.getY());
//            this->range.moveTo(pos.getX(),pos.getY());
//            this->weapon.movePosition(pos.getX(),pos.getY());
//        } else {
//            crash = true;
//        }
//    }

    while (!road.empty() && steps <= distance && !crash){
        Position pos = road.back();
        Size next_pos(pos.getX(),pos.getY(),
                        obj_size.getWidth(),obj_size.getHeight());
        if (compass.canIWalkToThisPosition(next_pos)) {
            double t_factor = compass.getTerrainFactorOn(pos.getX(),pos.getY());
            // move unit position, range and weapon
            this->obj_size.moveTo(pos.getX(),pos.getY());
            this->range.moveTo(pos.getX(),pos.getY());
            this->weapon.movePosition(pos.getX(),pos.getY());
            this->changed = true;
            this->action = MOVESTATE;
            road.pop_back();

            // increase or decrease distance til steps are more than unit speed
            if (steps <= unit_speed && unit_speed == 4) {
                distance = (int) (t_factor * distance);
            } else if (steps <= unit_speed && unit_speed > 4) {
                distance = (int) (t_factor *distance *
                                                (1-(damage_recv/life_points)));
            }
            ++steps;
        } else {
            crash = true;
        }
    }
    if (crash) {
        Position destiny = road.front();
        Position actual = obj_size.getPosition();
        road = compass.getFastestWay(actual,destiny);
        this->action = STANDINGSTATE;
        this->changed = true;
    }
}

void Unit::attack() {
    if (fire_count == 0 || fire_count == fire_rate) {
        fire_count = 0;
        // make a shot
        bullets.push_back(weapon.shotTarget(target));
        this->action = ATKSTATE;
        this->changed = true;
        fire_count = 0;
    } else {
        this->action = STANDINGSTATE;
        this->changed = true;
    }
    fire_count += 1;
}

std::string Unit::getActionState() const {
    return this->action;
}

Position Unit::getCurrentPosition() const {
    return this->obj_size.getPosition();
}

void Unit::setTargetToGrab(Teamable *object, std::string type) {
    // if its a flag any unit can grab it
    if (type == FLAGTYPE) {
        grab_target = object;
        this->state = GRABBINGSTATE;
    } else if (this->type == GRUNTTYPE && object->getTeam() == NEUTRAL
            && compass.checkIfItIsGrabbable(type)) {
        // Only Grunt robots can drive
        // If is not a flag, is a vehicle
        grab_target = object;
        this->state = GRABBINGSTATE;
        mount_vehicule = true;
    }
}

void Unit::grab() {
    this->state = STANDINGSTATE;
    this->action = STANDINGSTATE;
    this->changed = true;
    grab_target->changeTeam(this->team);
    if (mount_vehicule) {
        this->damage_recv = this->life_points;
    }
}

void Unit::setTargetToAttack(Occupant* target) {
    this->state = ATKSTATE;
    this->target = target;
    // clean bullets on weapon when a new target is set
    this->weapon.setNewTarget(target);
}

std::vector<Bullet*> Unit::collectBullets() {
    std::vector<Bullet*> tmp = bullets;
    bullets.clear();
    return tmp;
}

bool Unit::checkIfTargetIsOnRange() {
    bool on_range = true;
    Size trg_size = target->getSize();
    if (!range.isThereACollision(trg_size))
        on_range = false;
    else {
        weapon.recalculateRoadToTarget();
        std::vector<Position>& bullet_road = weapon.getBulletRoad();
        Size b_size = weapon.getBulletSize();
        if (!checkIfBulletWillHit(bullet_road,b_size))
            on_range = false;
    }

    return on_range;
}

bool Unit::checkIfBulletWillHit(std::vector<Position>& b_road, Size &b_size) {
    bool will_hit = true;
    for (auto x: b_road) {
        b_size.moveTo(x.getX(),x.getY());
        if (!compass.canBulletWalkToThisPosition(b_size,*this,*target))
            will_hit = false;
    }
    return will_hit;
}

bool Unit::doYouHaveAnyBullets() {
    return (!bullets.empty());
}

bool Unit::checkIfAlreadyOnMyWay(int x, int y) {
    bool on_my_way = false;
    if (state == MOVESTATE) {
        Position destiny = road.front();
        if (destiny.getX() == x && destiny.getY() == y)
            on_my_way = true;
    }
    return on_my_way;
}

Size Unit::getNextPosition(int steps) {
    Position dest = road.front();
    Position pos = road.back();

    while ((dest.getX() != pos.getX() || dest.getY() != pos.getY())
           && steps != 0) {
        road.pop_back();
        pos = road.back();
        --steps;
    }

    return Size(pos.getX(),pos.getY(),obj_size.getWidth(),obj_size.getHeight());
}

bool Unit::onRangeToGrabTarget() {
    Size trg_size = grab_target->getSize();
    return range.isThereACollision(trg_size);
}

