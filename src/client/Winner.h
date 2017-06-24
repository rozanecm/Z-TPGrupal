#ifndef Z_TPGRUPAL_WINNER_H
#define Z_TPGRUPAL_WINNER_H


#include "commands/Command.h"
#include "windows/GameWindow.h"

class Winner : public Command {
    bool& winner;
    GameWindow& window;

public:
    Winner(bool& winner, GameWindow& window);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_WINNER_H
