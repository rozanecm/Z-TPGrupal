//
// Created by rodian on 22/05/17.
//

#include "bullet.h"

Bullet::Bullet(std::string type, int dmg, int w_speed, Size& w_size,
               Occupant* target) : type(type), damage(dmg), w_speed(w_speed),
                          w_size(w_size), hit(false), target(target), id(0) {}

Bullet::Bullet(std::string type, int damage, int w_speed, Size& w_size) :
        type(type), damage(damage), w_speed(w_speed),
        w_size(w_size), hit(false), id(0) {}


void Bullet::shotTarget(Occupant* target) {
    this->target = target;
    calculateRoadToTarget();
}

bool Bullet::didHit() {
    return hit;
}

void Bullet::calculateRoadToTarget() {
    Position target_pos = target->getPosition();
    Position bullet_pos = w_size.getPosition();

    if (bullet_pos.getX() - target_pos.getX() != 0) {
        // solve the equation system getting the linear function y = ax + b
        double a = (bullet_pos.getY() - target_pos.getY()) /
                   (bullet_pos.getX() - target_pos.getX());
        double b = bullet_pos.getY() - (a * bullet_pos.getX());

        if (bullet_pos.getX() > target_pos.getX()) {
            for (int i = target_pos.getX(); i < target_pos.getX(); ++i) {
                road.push_back(calculateNextPosition(a, b, i));
            }
        } else if (bullet_pos.getX() < target_pos.getX()) {
            for (int i = target_pos.getX(); i > bullet_pos.getX(); --i) {
                road.push_back(calculateNextPosition(a, b, i));
            }
        }
    } else {
        // solve the equation system getting the linear function x = ay + b
        double a = (bullet_pos.getX() - target_pos.getX()) /
                (bullet_pos.getY() - target_pos.getY());
        double b = bullet_pos.getX() - (a * bullet_pos.getY());

        if (bullet_pos.getY() > target_pos.getY()) {
            for (int i = target_pos.getY(); i < bullet_pos.getY(); ++i) {
                road.push_back(calculateNextInvertPosition(a, b, i));
            }
        } else if (bullet_pos.getY() < target_pos.getY()) {
            for (int i = target_pos.getY(); i > bullet_pos.getY(); --i) {
                road.push_back(calculateNextInvertPosition(a, b, i));
            }
        }
    }
}

Position Bullet::calculateNextPosition(double a, double b, int x) {
    double temp_y = a * x + b;
    return Position(x, (int)temp_y);
}

Size Bullet::getSize() const {
    return this->w_size;
}

void Bullet::move() {
    if (!hit) {
        // cause target might be moving, recalculate road and then move
        calculateRoadToTarget();
        int distance = w_speed;
        int steps = 0;
        while (!road.empty() && steps <= distance) {
            Position pos = road.back();
            this->w_size.moveTo(pos.getX(), pos.getY());
            road.pop_back();
            ++steps;
        }
        // If you get the target, inflict damage
        Size targ_size = target->getSize();
        if (this->w_size.isThereACollision(targ_size)) {
            target->reduceLifeBy(damage);
            hit = true;
        }
    }
}

Position Bullet::calculateNextInvertPosition(double a, double b, int y) {
    double temp_x = a * y + b;
    return Position((int) temp_x, y);
}

bool Bullet::isRoadEmpty() {
    return road.empty();
}

std::vector<Position>& Bullet::getRoad() {
    return road;
}

void Bullet::damageThis(Occupant* other_target) {
    other_target->reduceLifeBy(damage);
    hit = true;
}

void Bullet::setStartLocation(int x, int y) {
    this->w_size.moveTo(x,y);
}

void Bullet::setCorrectId(int id) {
    this->id = id;
}

int Bullet::getId() const {
    return id;
}

Position Bullet::getPosition() const {
    return this->w_size.getPosition();
}

