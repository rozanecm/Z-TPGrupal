#ifndef Z_TPGRUPAL_COMMAND_H
#define Z_TPGRUPAL_COMMAND_H

#include <vector>
#include <string>

class Command {
public:
    virtual void execute(const std::vector<std::string> &args) = 0;
};

#endif //Z_TPGRUPAL_COMMAND_H
