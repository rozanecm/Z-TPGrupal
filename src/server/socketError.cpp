//
// Created by rodian on 22/05/17.
//

#include "socketError.h"

SocketError::SocketError(const char *message, ...) noexcept {
    strncpy(buffer, message, strlen(message));
}

const char* SocketError::what() const noexcept {
    return buffer;
}