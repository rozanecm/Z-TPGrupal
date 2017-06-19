//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_UNIT_H
#define Z_TPGRUPAL_UNIT_H

#include "Occupant.h"
#include "compass.h"
#include "teamable.h"
#include "bullet.h"
#include "weapon.h"

#define ATKSTATE "atk"
#define MOVESTATE "mv"
#define STANDINGSTATE "std"
#define FLAGTYPE "Flag"
#define GRUNTTYPE "Grunt"


class Unit: public Occupant {
private:
    Compass compass;
    Weapon weapon;
    int unit_speed, fire_rate, fire_count;
    // State of Unit can be "atk" if is attacking, "mv" if is moving, "std" if
    // is standing still
    std::string state;
    Size range;
    std::vector<Position> road;
    Occupant& target;
    std::vector<Bullet*> bullets;

public:
    Unit(int id, int life, std::string type, int unit_speed, Size size,
             Size range, Compass &compass, Weapon &weapon, int fire_rate);

    void makeAction();

    // Calculates the minimum road to destiny (x,y)
    void calculateRoadTo(int x, int y);

    // Indicates the Unit to make the next step on the road.
    // Make sure of use the calculateRoadTo method before this one.
    void move();

    void attack();

    // Returns the current position of the unit
    Position getCurrentPosition() const;

    // Returns "atk" if is attacking, "mv" if is moving, "std" if
    // is standing still
    std::string getState() const;

    void grab(Teamable* object, std::string type);

    void setTargetToAttack(Occupant& target);
    //-> make check is i have explosive attack for buildings

    bool doYouHaveAnyBullets();

    std::vector<Bullet*> collectBullets();

    bool checkIfTargetIsOnRange();

    // The bullet will hit if there is no Occupant in the middle.
    // except for bridges
    bool checkIfBulletWillHit(std::vector<Position> *b_road, Size& b_size);

    void getOnRangeOf(int x, int y);

    bool checkIfAlreadyOnMyWay(int x, int y);

    Size getNextPosition(int& steps);
};


#endif //Z_TPGRUPAL_UNIT_H
