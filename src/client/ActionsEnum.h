#ifndef Z_TPGRUPAL_ACTIONSENUM_H
#define Z_TPGRUPAL_ACTIONSENUM_H

enum ActionsEnum {
    /* the numbers indicate the angle in which the unit is facing.
     * 0:   looking to the right
     * 45:  looking to upper right corner
     * 90:  looking up
     * 135: looking to upper left corner
     * 180: looking to the left
     * 225: looking to bottom left corner
     * 270: looking down
     * 315: looking to bottom right corner
     * */
    FIRE_0, FIRE_45, FIRE_90, FIRE_135, FIRE_180, FIRE_225, FIRE_270, FIRE_315,

    WALK_0, WALK_45, WALK_90, WALK_135, WALK_180, WALK_225, WALK_270, WALK_315,

    STAND_0, STAND_45, STAND_90, STAND_135, STAND_180, STAND_225, STAND_270,
    STAND_315
};

#endif //Z_TPGRUPAL_ACTIONSENUM_H
