#ifndef Z_TPGRUPAL_ADDNATURE_H
#define Z_TPGRUPAL_ADDNATURE_H


#include "Command.h"

class AddNature : public Command {
public:
    AddNature();
    void execute(const std::vector<std::string>& args);

};


#endif //Z_TPGRUPAL_ADDNATURE_H
