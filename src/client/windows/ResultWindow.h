#ifndef Z_TPGRUPAL_RESULTWINDOW_H
#define Z_TPGRUPAL_RESULTWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>

class ResultWindow : public Gtk::Window {
    Gtk::Button* menu;
    Gtk::Button* close;
    Gtk::Label* winner;
    Gtk::Label* loser;
public:
    ResultWindow(BaseObjectType *cobject,
                 const Glib::RefPtr<Gtk::Builder> &builder);
};


#endif //Z_TPGRUPAL_RESULTWINDOW_H
