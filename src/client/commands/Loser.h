#ifndef Z_TPGRUPAL_LOSER_H
#define Z_TPGRUPAL_LOSER_H


#include "Command.h"
#include "../windows/GameWindow.h"

class Loser : public Command {
    bool& loser;
    GameWindow& window;
public:
    Loser(bool& loser, GameWindow& window);
    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_LOSER_H
