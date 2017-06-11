#include <iostream>
#include "InitialWindow.h"

InitialWindow::InitialWindow(BaseObjectType *cobject,
                             const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{
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
        s = std::shared_ptr<Socket>(new Socket(addr_str.c_str(), port));
    } catch (std::exception& e) {
        std::cout << "Could not connect to specified addr/port" << std::endl;
        return;
    }
    std::cout << "Hola" << addr_str << port << name << std::endl;
}


