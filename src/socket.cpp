//
// Created by rodian on 22/05/17.
//

#include "socket.h"

Socket::Socket(char* argv[]) {
    this->setAddrInfo(argv);
    std::string id(argv[0]);

    if (id.front() == '.')
        id = id.substr(2);

    if (id == "client") {
        connectToServer();

    } else if (id == "server") {
        setSocket();
        bindAndListen(this->result);
    }
}

Socket::Socket(int file_d) : file_descriptor(std::move(file_d)) {}

void Socket::setAddrInfo(char* argv[]) {
    memset(&this->addr_pointer, 0, sizeof(struct addrinfo));
    this->addr_pointer.ai_family = AF_INET;
    this->addr_pointer.ai_socktype = SOCK_STREAM;
    this->addr_pointer.ai_flags = 0;
    int status = -1;

    std::string id(argv[0]);
    if (id.front() == '.')
        id = id.substr(2);

    if (id == "server") {
        status = getaddrinfo(NULL, argv[1], &this->addr_pointer, &this->result);
    } else if (id == "client") {
        status = getaddrinfo(argv[1], argv[2], &this->addr_pointer, &this->result);
    }
    if (status != 0) {
        throw SocketError("Error at getaddrinfo.\n");
    }
}

void Socket::connectToServer() {
    struct addrinfo* possible_address;
    int status = -1;
    for (possible_address = this->result; possible_address != NULL && status == -1;
         possible_address = possible_address->ai_next) {
        createSocketTcp(possible_address);
        status = connect(this->file_descriptor,
                         possible_address->ai_addr, possible_address->ai_addrlen);
    }
    freeaddrinfo(this->result);
    if (status == -1)
        throw SocketError("Connection with all addresses failed\n", strerror(errno));
}

void Socket::createSocketTcp(struct addrinfo* addr_pointer) {
    if ((this->file_descriptor = socket(addr_pointer->ai_family,
             addr_pointer->ai_socktype, addr_pointer->ai_protocol)) < 0) {
        throw SocketError("Socket creation failed");
    }
}

void Socket::setSocket() {
    createSocketTcp(this->result);
    int value = 1;
    int status = setsockopt(this->file_descriptor,
                            SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    if (status == -1) {
        close(this->file_descriptor);
        freeaddrinfo(this->result);
        throw SocketError("Error while setting up socket: \n", strerror(errno));
    }
}

int Socket::socketSend(const char* buff_to_send, size_t length) {
    size_t bytes_sent = 0;
    int send_state = 1;

    std::string message(buff_to_send);

    while ((bytes_sent < length) && (send_state)) {
        send_state = send(this->file_descriptor,
                          (buff_to_send + bytes_sent),
                          length - bytes_sent, MSG_NOSIGNAL);
        if (send_state < 0) {
            return send_state;
        } else if (send_state > 0) {
            bytes_sent += send_state;
        }
    }
    return bytes_sent;
}

int Socket::socketRecieve(char* buff_to_receive, size_t length) {
    size_t bytes_received = 0;
    int receive_state = 1;
    while ((bytes_received < length) && (receive_state)) {
        receive_state = recv(this->file_descriptor,
                             buff_to_receive + bytes_received,
                             length - bytes_received, MSG_NOSIGNAL);
        if (receive_state < 0) {
            return receive_state;
        }
        bytes_received += receive_state;
    }
    return bytes_received;
}

void Socket::bindAndListen(struct addrinfo *addr_pointer) {
    int status = bind(this->file_descriptor, addr_pointer->ai_addr,
                      addr_pointer->ai_addrlen);
    if (status == -1) {
        close(this->file_descriptor);
        throw SocketError("Error while binding: \n", strerror(errno));
    }

    if ((status = listen(this->file_descriptor, 10)) == -1) {
        close(this->file_descriptor);
        throw SocketError("Error while listening: \n", strerror(errno));
    }
    freeaddrinfo(this->result);
}

Socket Socket::socketAccept() {
    struct sockaddr_in client;
    socklen_t clilen = (socklen_t) sizeof(struct sockaddr_in);
    int connected_socket = 0;

    connected_socket = accept(this->file_descriptor,
                              (struct sockaddr *) &client, &clilen);
    Socket remote_socket(connected_socket);
    // remote_socket.file_descriptor = connected_socket;

    return remote_socket;;
}

bool Socket::isValid() {
    return (this->file_descriptor >= 0);
}

void Socket::socketShutDownForRead() {
    shutdown(this->file_descriptor, SHUT_RD);
}

void Socket::socketShutDownForWrite() {
    shutdown(this->file_descriptor, SHUT_WR);
}

Socket::Socket(Socket&& other) {
    this->file_descriptor = other.file_descriptor;
    other.file_descriptor = -1;
}

Socket::~Socket() {
    if (this->file_descriptor > 0)
        close(this->file_descriptor);
}
