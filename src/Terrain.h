#ifndef Z_TPGRUPAL_TERRAIN_H
#define Z_TPGRUPAL_TERRAIN_H

#include <iostream>
#include <string>

// Class Terrain to be set on every cell
class Terrain {
    private:
        std::string kind;
        double factor;

    public:
        // Terrain is of constant kind and factor for moving over it
        Terrain(std::string& kind, int factor );

        // Returns string to the kind of terrain
        std::string getKind() const;

        // Returns the moving factor
        double getFactor() const;

        ~Terrain();
};


#endif //Z_TPGRUPAL_TERRAIN_H
