#ifndef Z_TPGRUPAL_UPDATE_H
#define Z_TPGRUPAL_UPDATE_H


#include "Command.h"

class Update : public Command {
public:
    Update();

    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_UPDATE_H
