//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_MAP_H
#define Z_TPGRUPAL_MAP_H


#include <vector>
#include "cell.h"
#include "Occupant.h"
// later written
//class Compass;
class Unit;

class Map {
private:
    std::vector<std::vector<Cell>> terrain_map;
    Size map_size;
    std::vector<Occupant*>* all_occupants;
    std::string xml;
    /*std::vector<Unit>& all_units;*/

public:
    // Map receives the center position (x,y) and dimensions width and height
    Map(int x, int y, int width, int height,
        std::vector<std::vector<Cell>>& terrain_map,
        std::vector<Occupant*>* all_occupants,
        std::string& xml
    );

//    void addUnits(std::vector<Unit>& all_units);
    // I'm gonna add Occupants on Game
    // Adds a new Occupant to the list of Occupants of the map
//    void addOccupant(Occupant* new_occupant);

    // Recieves the coordinates (x,y) and returns the terrain factor on that
    // position on the map.
    double getTerrainFactorOn(int x, int y);

    // Returns the name of the type of Terrain
    std::string getTerrainType(int x, int y);

    // Returns true if the points are empty
    bool areThisPointsEmpty(Size& size);

    // Returns true if points are empty or it is the Occupant
    bool areThisPointsEmpty(Size& size, Occupant& occupant);

    // Recieves the size of an object on the position that wants to be walk
    // Returns true if the object fits and can step to that position
    bool canIWalkToThisPosition(Size& size);

    // Recieves the size of an object on the position that wants to be walk
    // Returns true if the object fits and can step to that position ignoring
    // the occupant from parameter
    bool canBulletWalkToThisPosition(Size& size, Occupant& other);

    // Returns the width of the map
    int getWidth();

    // Returns the Heigth of the map
    int getHeigth();

    bool doesThisPositionExist(int x, int y);

    bool isThereLava(Size& other_size);

    bool thereIsABridge(Size& other_size);

    std::string& get_map();

    void getAClosePlaceNextTo(Size& u_size, Size& fac_size);

    std::vector<Occupant*>& getOccupants();

    void updateOccupants(std::vector<Occupant*>* all_occupants);
};


#endif //Z_TPGRUPAL_MAP_H
