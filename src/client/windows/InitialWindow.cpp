#include <iostream>
#include "InitialWindow.h"

#define ERROR_MSG "error"
#define OK_MSG "ok"

InitialWindow::InitialWindow(BaseObjectType *cobject,
                             const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::Window(cobject) {
    builder->get_widget("AddressEntry", address_entry);
    builder->get_widget("PortEntry", port_entry);
    builder->get_widget("NameEntry", name_entry);
    builder->get_widget("ConnectButton", connect);
    connect->signal_clicked().connect(sigc::mem_fun(*this,
                                                    &InitialWindow::on_click));
}

void InitialWindow::on_click() {
    std::string addr_str = address_entry->get_text();
    std::string port_str = port_entry->get_text();
    name = name_entry->get_text();

    int port = 0;
    try {
        port = std::stoi(port_str);
        Socket s(addr_str.c_str(), port);
        messenger = std::shared_ptr<ServerMessenger>(new ServerMessenger(s));
    } catch (SocketError &e) {
        std::cout << "Could not connect to specified addr/port" << std::endl;
        return;
    }
    messenger.get()->send(name);
    std::string response = messenger.get()->receive();
    if (response == ERROR_MSG) {
        std::cout << "A player with this name already exists" << std::endl;
        return;
    }
    this->hide();
}

std::shared_ptr<ServerMessenger> InitialWindow::get_socket() {
    return messenger;
}

const std::string &InitialWindow::get_username() {
    return name;
}


