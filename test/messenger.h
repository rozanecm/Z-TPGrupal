#ifndef TP3TALLER_PROTOCOL_SOCKET_H
#define TP3TALLER_PROTOCOL_SOCKET_H

#include <string>
#include "socket.h"


/* Sends 'len' bytes of data through Socket 's', taken from he buffer 'msg'.
 * Complies with the assignment protocol of sending the message length through
 * the socket first, then the actual message. */
void protocol_send(Socket& s, const char* msg, unsigned int len);

/* Endpoint to receive the data passed from the function protocol_send. Parses
 * the result into a std::string type and returns it. */
std::string protocol_receive(Socket& s);

#endif //TP3TALLER_PROTOCOL_SOCKET_H
