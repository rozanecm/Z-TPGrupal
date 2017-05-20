#include <gtkmm/builder.h>
#include <giomm.h>
#include <gdkmm.h>
#include "GameArea.h"

GameArea::GameArea() : flagCounter(0){
    /* load blue flag imgs */
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_0.png"));
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_1.png"));
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_2.png"));
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_3.png"));
}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    drawFlagAnimation(cr);
    return true;
}

void GameArea::drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gdk::Cairo::set_source_pixbuf(cr, blueFlagVector.at(flagCounter), 100, 80);
    cr->rectangle(110, 90, blueFlagVector.at(flagCounter)->get_width(),
                  blueFlagVector.at(flagCounter)->get_height());
    cr->fill();
    if (flagCounter == blueFlagVector.size()-1){
        flagCounter = 0;
    }else{
        flagCounter++;
    }
}
