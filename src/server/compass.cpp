//
// Created by rodian on 14/05/17.
//

#include "compass.h"

#define SIDEWALK 10
#define DIAGONALWALK 14
#define HMIN 100

Compass::Compass(Map &map, Size &unit_size, int unit_id, int unit_speed)
        : map(map),
          unit_size(unit_size), unit_id(unit_id) ,unit_speed(unit_speed) {
    this->buildNodeMap();
    this->setTerrainModifier();
}

void Compass::setTerrainModifier() {
    terrain_modifier.insert(std::pair<std::string,int>("Carretera",1));
    terrain_modifier.insert(std::pair<std::string,int>("Camino Asfaltado",1));
    terrain_modifier.insert(std::pair<std::string,int>("Tierra",2));
    terrain_modifier.insert(std::pair<std::string,int>("Pradera",2));
    terrain_modifier.insert(std::pair<std::string,int>("Nieve",2));
    terrain_modifier.insert(std::pair<std::string, int>("Agua", 3));
}

void Compass::buildNodeMap() {
    // the nodes has the size of the unit that is using this compass
    for(int it = 0; it < map.getWidth(); ++it) {
        std::vector<Node*> row_vec;
        astar_map.push_back(row_vec);
        for(int jt = 0; jt < map.getHeigth(); ++jt) {
            astar_map.back().push_back(new Node(it, jt,
                                  unit_size.getWidth(), unit_size.getHeight()));
        }
    }
}

std::vector<Position> Compass::getFastestWay(Position& from, Position& to) {
    this->road.clear();

    // check if it's a possible position
    Position destiny = getAValidPositionForDestiny(to);
    // if I'm already on the closest position return it
    if (from.getX() == destiny.getX() && from.getY() == destiny.getY()) {
        this->road.push_back(destiny);
        return road;
    } else {
        // set H value for destiny
        this->setHValueForDestiny(destiny);

        // start algorithm
        // add "from" to visited list
        Node *start_node = astar_map[from.getX()][from.getY()];
        std::string terrain_type = map.getTerrainType(from.getX(), from.getY());
        start_node->setGValue(0, terrain_modifier[terrain_type]);
        start_node->setNewParent(start_node);
        this->closed_nodes.push_back(start_node);

        Node *closer_node = start_node;
        // While haven't reach destiny node or open_nodes has nodes to visit.
        bool finished = false;
        bool open_nodes_empty = false;

        while (!finished && (!open_nodes_empty)) {
            // get adjacent's and add them to looking list in order of F value.
            // On tie use H value.

            this->getAdjacents(closer_node);

            // if there are no adjacent's and open_node is empty, end search
            if (open_nodes.empty()) {
                open_nodes_empty = true;
            } else {
                // get the minimum F and add it to visit list
                // (remove from looking list)
                closer_node = open_nodes.back();
                open_nodes.pop_back();
                this->closed_nodes.push_back(closer_node);

                // check if destiny is between them
                if (closed_nodes.back()->getHvalue() == 0)
                    finished = true;
            }
        }
        Node *closest;
        if (finished) {
            this->getRoad(from, closer_node);
        } else {
            closest = this->searchForClosestNode();
            this->getRoad(from, closest);
        }
        return road;
    }
}

void Compass::setHValueForDestiny(Position& to) {
    astar_map[to.getX()][to.getY()]->setHValue(0);

    for (auto x: astar_map) {
        for(auto y: x){
            Position tmp = y->getPosition();
            int h_value = HMIN * (this->getModuleOfSubtraction(tmp.getX(),
            to.getX()) + this->getModuleOfSubtraction(tmp.getY(),to.getY()));
            y->setHValue(h_value);
        }
    }
}

void Compass::getAdjacents(Node *node) {
    // get limits
    int x_min = node->getPosition().getX() - 1;
    int x_max = node->getPosition().getX() + 1;
    int y_min = node->getPosition().getY() - 1;
    int y_max = node->getPosition().getY() + 1;

    bool adj_new_g;
    Node* adj;
    for (int x_pos = x_min; x_pos <= x_max; ++x_pos) {
        for (int y_pos = y_min; y_pos <= y_max; ++y_pos) {
            if (map.doesThisPositionExist(x_pos, y_pos)){
                adj = astar_map[x_pos][y_pos];
                Size size = adj->getSize();

                // Check if whether node fit or the position is not available.
                // Also discard the node looking for his adjacent
                if ((map.canIWalkToThisPosition(size, unit_id)) &&
                    this->isNotMe(node, adj)) {

                    // G value differs when the node is diagonal or next to it
                    if (this->isThisNodeOnDiagonal(node, adj)) {
                        adj_new_g = this->writeGandSetParent(node, adj,
                                                             DIAGONALWALK);

                    } else {
                        adj_new_g = this->writeGandSetParent(node, adj,
                                                             SIDEWALK);
                    }
                    if (adj_new_g)
                        this->addToOpenInOrder(adj);
                }
            }
        }
    }
}

bool Compass::isThisNodeOnDiagonal(Node* ref, Node* other) {
    Position pos_ref = ref->getPosition();
    Position pos_other = other->getPosition();
    int diff_y = getModuleOfSubtraction(pos_ref.getY(),pos_other.getY());
    int diff_x = getModuleOfSubtraction(pos_ref.getX(),pos_other.getX());
    return ((diff_x > 0) && (diff_y > 0));
}

bool Compass::isNotMe(Node* node, Node* other) {
    Position ref = node->getPosition();
    Position ady = other->getPosition();
    return !((ref.getX() == ady.getX()) && (ref.getY() == ady.getY()));
}

void Compass::addToOpenInOrder(Node* new_node) {
    // Only add to the vector those that haven't been seen
    if (!new_node->beenSeen()){
        this->insertNodeOnOpen(new_node);
    } else {
        this->changeNodePosition(new_node);
    }
}

bool Compass::writeGandSetParent(Node* ref, Node* adj, int walk) {
    int new_g = walk + ref->getGValue();
    bool adj_change_g = false;
    // if F value from node is lower than previous or this
    // adjacent hasn't been seen yet,
    // add new g value and change parent.
    Position pos = adj->getPosition();
    std::string terrain_type = map.getTerrainType(pos.getX(),pos.getY());
    Size adj_size = adj->getSize();
    // when is a vehicle and it's water, don't add it to open list
    if (!(unit_speed != 4 && terrain_type == "Agua" &&
            !map.thereIsABridge(adj_size))) {
        int terrain_factor = terrain_modifier[terrain_type];
        if ((adj->beenSeen() &&
             (adj->getFValueIfGWere(new_g, terrain_factor) <
              adj->getFValue())) ||
            (!adj->beenSeen())) {
            adj->setGValue(new_g, terrain_factor);
            adj->setNewParent(ref);
            adj_change_g = true;
        }
    }
    return adj_change_g;
}

void Compass::changeNodePosition(Node *node) {
    // first erase node from vector
    bool erased = false;
    Position node_pos = node->getPosition();
    std::vector<Node *>::iterator it = open_nodes.begin();
    while ((!erased) && (it != open_nodes.end())) {
        Position it_pos = (*it)->getPosition();
        if ((it_pos.getX() == node_pos.getX()) &&
                (it_pos.getY() == node_pos.getY())){
            it = open_nodes.erase(it);
            erased = true;
        } else {
            ++it;
        }
    }
    // Add it again in correct position
    this->insertNodeOnOpen(node);
}

void Compass::insertNodeOnOpen(Node *new_node) {
    if (new_node->getHvalue() == 0) {
        open_nodes.push_back(new_node);
    } else {
        bool inserted = false;
        // Save nodes by F value. The lowest on the back.
        // If two nodes have same F value, the one with the lowest H value
        // will be closer to the back.
        std::vector<Node *>::iterator it = open_nodes.begin();
        while ((!inserted) && (it != open_nodes.end())) {
            if (((*it)->getFValue()) < new_node->getFValue()) {
                open_nodes.insert(it, new_node);
                inserted = true;
            } else if (((*it)->getFValue()) == new_node->getFValue()) {
                if (((*it)->getHvalue()) < new_node->getHvalue()) {
                    open_nodes.insert(it, new_node);
                    inserted = true;
                }
            }
            ++it;
        }
        if (!inserted) {
            open_nodes.push_back(new_node);
        }
    }
    this->checkIfIsDestinyNeighbor(new_node);
}

void Compass::getRoad(Position& from,Node *destiny) {
    road.push_back(destiny->getPosition());
    Node* next_node = destiny->getParent();

    Position current_pos = next_node->getPosition();
    while ((current_pos.getX() != from.getX()) ||
            (current_pos.getY() != from.getY())) {
        road.push_back(current_pos);
        next_node = next_node->getParent();
        current_pos = next_node->getPosition();
    }
}

Node *Compass::searchForClosestNode() {
    Node* closest = closed_nodes.front();
    for (auto x: closed_nodes){
        if ((x->getHvalue() < closest->getHvalue()) ||
         ((x->getHvalue() == closest->getGValue()) &&
          (x->getFValue() < closest->getFValue()))) {
            closest = x;
        }
    }
    return closest;
}

int Compass::getModuleOfSubtraction(int x, int y) {
    if ((x - y) > 0)
        return x - y;
    return y - x;
}

Compass::~Compass() {
    if (!astar_map.empty()) {
        int j = 0;
        for (auto x: astar_map) {
            int i = 0;
            for (auto y: x) {
//                delete (y);
                ++i;
            }
            ++j;
        }
    }
}

void Compass::checkIfIsDestinyNeighbor(Node* node) {
    if ((node->getHvalue() <= HMIN*2) && (node->getHvalue() != 0)){
        // get limits
        int x_min = node->getPosition().getX() - 1;
        int x_max = node->getPosition().getX() + 1;
        int y_min = node->getPosition().getY() - 1;
        int y_max = node->getPosition().getY() + 1;

        Node *adj;
        bool adj_new_g;
        for (int x_pos = x_min; x_pos <= x_max; ++x_pos) {
            for (int y_pos = y_min; y_pos <= y_max; ++y_pos) {
                if (map.doesThisPositionExist(x_pos, y_pos)) {
                    adj = astar_map[x_pos][y_pos];
                    Size size = adj->getSize();
                    if (adj->getHvalue() == 0) {
                        // G value differs when the node is diagonal
                        // or next to it
                        if ((map.canIWalkToThisPosition(size, unit_id)) &&
                            this->isNotMe(node, adj)) {
                            if (this->isThisNodeOnDiagonal(node, adj)) {
                                adj_new_g = this->writeGandSetParent(node, adj,
                                                                 DIAGONALWALK);
                            } else {
                                adj_new_g = this->writeGandSetParent(node, adj,
                                                                     SIDEWALK);
                            }
                            if (adj_new_g)
                                this->addToOpenInOrder(adj);
                        }
                    }
                }
            }
        }
    }
}

bool Compass::canIWalkToThisPosition(Size &size) {
    return map.canIWalkToThisPosition(size, unit_id);
}

double Compass::getTerrainFactorOn(int x, int y) {
    return map.getTerrainFactorOn(x,y);
}

bool Compass::canBulletWalkToThisPosition(Size &size, Occupant &occupant) {
    return map.canBulletWalkToThisPosition(size,occupant);
}

void Compass::changeUnitSize(Size &new_size) {
    this->unit_size = new_size;
}

void Compass::changeUnitSpeed(int speed) {
    this->unit_speed = speed;
}

Position Compass::getAValidPositionForDestiny(Position &destiny) {
    Node *dest = astar_map[destiny.getX()][destiny.getY()];
    Size size = dest->getSize();
    if (map.canIWalkToThisPosition(size, unit_id)) {
        return destiny;
    } else {
        return getClosestValidPosition(destiny);
    }
}

Position Compass::getClosestValidPosition(Position &pos) {
    bool found = false;
    int i = 1;
    Node* closest_node = astar_map[pos.getX()][pos.getY()];
    while (!found) {
        int x_min = pos.getX() - i;
        int x_max = pos.getX() + i;
        int y_min = pos.getY() - i;
        int y_max = pos.getY() + i;

        for (int x_pos = x_min; x_pos <= x_max; ++x_pos) {
            if (map.doesThisPositionExist(x_pos, y_max)) {
                Node *tmp = astar_map[x_pos][y_max];
                Size size = tmp->getSize();
                std::string terrain_type = map.getTerrainType(x_pos,y_max);
                // if you fit on the position. When it's a vehicule check
                // if it's different to water.
                if ((map.canIWalkToThisPosition(size, unit_id)) &&
                    (!(unit_speed != 4 && terrain_type == "Agua" &&
                   !map.thereIsABridge(size)))) {
                    found = true;
                    closest_node = tmp;
                    break;
                }
            }
        }

        if (!found) {
            for (int x_pos = x_min; x_pos <= x_max; ++x_pos) {
                if (map.doesThisPositionExist(x_pos, y_min)) {
                    Node *tmp = astar_map[x_pos][y_min];
                    Size size = tmp->getSize();
                    std::string terrain_type = map.getTerrainType(x_pos,y_min);
                    // if you fit on the position. When it's a vehicule check
                    // if it's different to water.
                    if ((map.canIWalkToThisPosition(size,unit_id)) &&
                        (!(unit_speed != 4 && terrain_type == "Agua" &&
                           !map.thereIsABridge(size)))) {
                        found = true;
                        closest_node = tmp;
                        break;
                    }
                }
            }
        }

        if (!found) {
            for (int y_pos = y_min; y_pos <= y_max; ++y_pos) {
                if (map.doesThisPositionExist(x_max, y_pos)) {
                    Node *tmp = astar_map[x_max][y_pos];
                    Size size = tmp->getSize();
                    std::string terrain_type = map.getTerrainType(x_max,y_pos);
                    // if you fit on the position. When it's a vehicule check
                    // if it's different to water.
                    if ((map.canIWalkToThisPosition(size, unit_id)) &&
                        (!(unit_speed != 4 && terrain_type == "Agua" &&
                           !map.thereIsABridge(size)))) {
                        found = true;
                        closest_node = tmp;
                        break;
                    }
                }
            }
        }

        if (!found) {
            for (int y_pos = y_min; y_pos <= y_max; ++y_pos) {
                if (map.doesThisPositionExist(x_min, y_pos)) {
                    Node *tmp = astar_map[x_min][y_pos];
                    Size size = tmp->getSize();
                    std::string terrain_type = map.getTerrainType(x_min,y_pos);
                    // if you fit on the position. When it's a vehicule check
                    // if it's different to water.
                    if ((map.canIWalkToThisPosition(size, unit_id)) &&
                        (!(unit_speed != 4 && terrain_type == "Agua" &&
                           !map.thereIsABridge(size)))) {
                        found = true;
                        closest_node = tmp;
                        break;
                    }
                }
            }
        }

        ++i;
    }
    return closest_node->getSize().getPosition();
}

void Compass::changeUnitId(int id) {
    this->unit_id = id;
}
