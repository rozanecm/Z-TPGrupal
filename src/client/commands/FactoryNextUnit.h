#ifndef Z_TPGRUPAL_FACTORYNEXTUNIT_H
#define Z_TPGRUPAL_FACTORYNEXTUNIT_H


#include "Command.h"
#include "../windows/GameWindow.h"

class FactoryNextUnit : public Command {

    GameWindow& window;
public:
    FactoryNextUnit(GameWindow &window);
    void execute(const std::vector<std::string>& args);
};


#endif //Z_TPGRUPAL_FACTORYNEXTUNIT_H
