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

    bool back_to_menu;
public:
    ResultWindow(BaseObjectType *cobject,
                 const Glib::RefPtr<Gtk::Builder> &builder);

    void display_lose_screen();
    void display_win_screen();

    void menu_click();

    void close_click();

    bool go_back_to_menu();
};


#endif //Z_TPGRUPAL_RESULTWINDOW_H
