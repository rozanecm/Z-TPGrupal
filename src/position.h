//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_POSITION_H
#define Z_TPGRUPAL_POSITION_H

// Position class to keep the coordenates of the map
class Position {
private:
    int x, y;
public:
    Position(int x, int y);

    int getX() const;

    int getY() const;

    void moveTo(int x, int y);
};


#endif //Z_TPGRUPAL_POSITION_H
