#ifndef Z_TPGRUPAL_CAMERA_H
#define Z_TPGRUPAL_CAMERA_H


#include <utility>

class Camera {
public:
    Camera(unsigned int tileSize, unsigned int mapWidth,
               unsigned int mapHeight);

    std::pair<unsigned int, unsigned int> getPosition();

    void setMapWidth(unsigned int width);

    void setMapHeight(unsigned int height);

private:
    /* save tile size in pixels so calculations on max and min coord.
     * can be done */
    unsigned int tileSize;

    /* position in pixels */
    std::pair<unsigned int, unsigned int> position;

    unsigned int minXCoordinate, minYCoordinate, maxXCoordinate, maxYCoordinate;

    unsigned int mapWidth, mapHeight;
};


#endif //Z_TPGRUPAL_CAMERA_H
