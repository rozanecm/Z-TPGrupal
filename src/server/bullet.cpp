//
// Created by rodian on 22/05/17.
//

#include "bullet.h"

Bullet::Bullet(std::string type, int dmg, int w_speed, Size w_size,
               Occupant target) : type(type), damage(dmg), w_speed(w_speed),
                                  w_size(w_size), hit(false), target(target) {}

void Bullet::shotTarget(Occupant& target) {
    this->target = target;
    calculateRoadToTarget();
}

bool Bullet::didHit() {
    return hit;
}

void Bullet::calculateRoadToTarget() {
    Position target_pos = target.getPosition();
    Position bullet_pos = w_size.getPosition();

    if (bullet_pos.getX() - target_pos.getX() != 0) {
        // solve the equation system getting the linear function y = ax + b
        double a = (bullet_pos.getY() - target_pos.getY()) /
                   (bullet_pos.getX() - target_pos.getX());
        double b = bullet_pos.getY() - (a * bullet_pos.getX());

        if (bullet_pos.getX() > target_pos.getX()) {
            for (int i = bullet_pos.getX(); i >= target_pos.getX(); --i) {
                road->push_back(calculateNextPosition(a, b, i));
            }
        } else if (bullet_pos.getX() < target_pos.getX()) {
            for (int i = bullet_pos.getX(); i <= target_pos.getX(); ++i) {
                road->push_back(calculateNextPosition(a, b, i));
            }
        }
    } else {
        // solve the equation system getting the linear function x = ay + b
        double a = (bullet_pos.getX() - target_pos.getX()) /
                (bullet_pos.getY() - target_pos.getY());
        double b = bullet_pos.getX() - (a * bullet_pos.getY());

        if (bullet_pos.getY() > target_pos.getY()) {
            for (int i = bullet_pos.getY(); i >= target_pos.getY(); --i) {
                road->push_back(calculateNextInvertPosition(a, b, i));
            }
        } else if (bullet_pos.getY() < target_pos.getY()) {
            for (int i = bullet_pos.getY(); i <= target_pos.getY(); ++i) {
                road->push_back(calculateNextInvertPosition(a, b, i));
            }
        }
    }
}

Position Bullet::calculateNextPosition(double a, double b, int x) {
    double temp_y = a * x + b;
    return Position(x, (int)temp_y);
}

Size Bullet::getSize() {
    return this->w_size;
}

void Bullet::move() {

}

Position Bullet::calculateNextInvertPosition(double a, double b, int y) {
    double temp_x = a * y + b;
    return Position((int) temp_x, y);
}
