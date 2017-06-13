#ifndef Z_TPGRUPAL_CAMERA_H
#define Z_TPGRUPAL_CAMERA_H


#include <utility>

class Camera {
private:
    /* save tile size in pixels so calculations on max and min coord.
     * can be done */
    unsigned int tileSize;

    unsigned int numberOfTilesToShow;

    /* position in pixels */
    std::pair<unsigned int, unsigned int> position;

    unsigned int minXCoordinate, minYCoordinate, maxXCoordinate, maxYCoordinate;

    unsigned int mapWidth, mapHeight;

public:
    /* camera will be initialized in (minx, miny) position */
    Camera(unsigned int tileSize, unsigned int mapWidth, unsigned int mapHeight,
               unsigned int numberOfTilesToShow);

    std::pair<unsigned int, unsigned int> getPosition();

    void setMapWidth(unsigned int width);

    void setMapHeight(unsigned int height);

    void moveUp();

    void moveDown();

    void moveLeft();

    void moveRight();

    std::pair<unsigned int, unsigned int> cameraOffset();

    unsigned int mapToCameraXCoordinate(unsigned int globalXPosition);

    unsigned int mapToCameraYCoordinate(unsigned int globalYPosition);
};


#endif //Z_TPGRUPAL_CAMERA_H
