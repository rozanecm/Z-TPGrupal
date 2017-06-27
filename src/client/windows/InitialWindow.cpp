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
    if (messenger.get()) { // Connection already established
        send_name();
        return;
    }

    try {
        int port = 0;
        port = std::stoi(port_str);
        Socket s(addr_str.c_str(), port);
        messenger = std::shared_ptr<ServerMessenger>(new ServerMessenger(s));
        send_name();
    } catch (SocketError &e) {
        std::cerr << "Could not connect to specified addr/port" << std::endl;
        return;
    }
}

void InitialWindow::send_name() {
    messenger.get()->send("changename-" + name);
    std::string response = messenger.get()->receive();
    if (response == ERROR_MSG) {
        std::cerr << "A player with this name already exists" << std::endl;
        return;
    }
    hide();
}

std::shared_ptr<ServerMessenger> InitialWindow::get_socket() {
    return messenger;
}

const std::string &InitialWindow::get_username() {
    return name;
}


