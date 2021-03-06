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
#define GRABBINGSTATE "grb"
#define FLAGTYPE "flag"
#define GRUNTTYPE "grunt"


class Unit: public Occupant {
private:
    Compass* compass;
    Weapon weapon;
    int unit_speed, fire_rate, fire_count;
    std::string state,action;
    Size range, grab_range;
    std::vector<Position> road;
    Occupant* target;
    Teamable* grab_target;
    std::vector<Bullet*> bullets;
    bool got_target, mount_vehicule;

public:
    Unit(int id, int life, std::string type, int unit_speed, Size size,
             Size range, Compass* compass, Weapon &weapon, int fire_rate);

    // This method is the one who makes the unit make the action that he must
    // do depending on his state
    void makeAction();

    // Calculates the minimum road to destiny (x,y)
    void calculateRoadTo(int x, int y);

    // Returns the current position of the unit
    Position getCurrentPosition() const;

    // Returns "atk" if is attacking, "mv" if is moving, "std" if
    // is standing still
    std::string getActionState() const;

    void grab();

    void setTargetToAttack(Occupant* target);

    void setTargetToGrab(Teamable* object, std::string type);

    bool doYouHaveAnyBullets();

    std::vector<Bullet*> collectBullets();

    bool checkIfTargetIsOnRange();

    // The bullet will hit if there is no Occupant in the middle.
    // except for bridges
    bool checkIfBulletWillHit(std::vector<Position>& b_road, Size& b_size);

    void getOnRangeOf(int x, int y);

    bool checkIfAlreadyOnMyWay(int x, int y);

    Size getNextPosition(int steps);

    void recalculateMyStartPosition();

    ~Unit();

private:
    // Indicates the Unit to make the next step on the road.
    // Make sure of use the calculateRoadTo method before this one.
    void move();

    // this method creates the bullets to attack a certain target
    void attack();

    bool onRangeToGrabTarget();
};


#endif //Z_TPGRUPAL_UNIT_H
