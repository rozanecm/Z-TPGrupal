//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_SOCKETERROR_H
#define Z_TPGRUPAL_SOCKETERROR_H

#include <iostream>
#include <cstring>

////////////////////////
// SocketError Class to warn
// of an error on the socket
////////////////////////

class SocketError : public std::exception {
private:
    char buffer[124];

public:
    explicit SocketError(const char* message, ...) noexcept;

    // Returns the error message
    virtual const char* what() const noexcept;
};

#endif //Z_TPGRUPAL_SOCKETERROR_H
