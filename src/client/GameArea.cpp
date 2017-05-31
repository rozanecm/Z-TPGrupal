#include <gtkmm/builder.h>
#include <utility>
#include <gdkmm.h>
#include <iostream>
#include "GameArea.h"
#include <string>


#define TILESIZE 16    //tile width in pixels
#define NUMBER_OF_TILES_TO_SHOW 10

GameArea::GameArea(BaseObjectType *cobject,
                   const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::DrawingArea(cobject),
        flagCounter(0),
        playersMonitor(nullptr),
        buildingsMonitor(nullptr),
        mapMonitor(nullptr),
        /* camera is initialized with size 0,0 because we dont
         * have this data yet */
        camera(TILESIZE, 0, 0, NUMBER_OF_TILES_TO_SHOW) {
    /* load blue flag imgs */
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/flag_blue_n00.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/flag_blue_n01.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/flag_blue_n02.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/flag_blue_n03.png"));

    /* load some img */
    someImg = Gdk::Pixbuf::create_from_file("res/portraits/grunt.png");

    /* Load tiles */
    tiles["Tierra"] = Gdk::Pixbuf::create_from_file
            ("res/assets/tiles/tierra.png");
    tiles["Agua"] = Gdk::Pixbuf::create_from_file
            ("res/assets/tiles/agua.png");
    tiles["Lava"] = Gdk::Pixbuf::create_from_file("res/assets/tiles/lava.png");

    add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    add_events(Gdk::EventMask::KEY_PRESS_MASK);
    set_can_focus(true);
}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    drawBaseMap(cr, camera.getPosition());
    drawFlagAnimation(cr, 500, 500);
    return true;
}

void GameArea::drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr,
                           std::pair<unsigned int, unsigned int>
                           cameraPosition) {
    /* cameraPosition is given in pixels.
     * i,j indicate TILES. */
    for (unsigned int i = 0; i < NUMBER_OF_TILES_TO_SHOW; ++i){
        for (unsigned int j = 0; j < NUMBER_OF_TILES_TO_SHOW; ++j){
            drawTileAt(cr, i, j, mapMonitor->getTerrainTypeAt(
                    cameraPosition.first/TILESIZE-NUMBER_OF_TILES_TO_SHOW/2 + i,
                    cameraPosition.second/TILESIZE-NUMBER_OF_TILES_TO_SHOW/2 +
                            j));
        }
    }
}

void GameArea::drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                          unsigned int xCoordinate, unsigned int yCoordinate,
                          std::string terrainType) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, tiles.at(terrainType),
                                  xCoordinate*get_width()/
                                  NUMBER_OF_TILES_TO_SHOW,
                                  yCoordinate*get_height()/
                                  NUMBER_OF_TILES_TO_SHOW);

    cr->rectangle(xCoordinate * get_width()/NUMBER_OF_TILES_TO_SHOW,
                  yCoordinate * get_height()/NUMBER_OF_TILES_TO_SHOW,
                  get_width()/NUMBER_OF_TILES_TO_SHOW,
                  get_height()/NUMBER_OF_TILES_TO_SHOW);
    cr->fill();
    cr->restore();
}

void GameArea::drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &cr,
                                 int xCoordinate, int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, blueFlagVector.at(flagCounter),
                                  xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  blueFlagVector.at(flagCounter)->get_width(),
                  blueFlagVector.at(flagCounter)->get_height());
    cr->fill();
    if (flagCounter == blueFlagVector.size()-1){
        flagCounter = 0;
    }else{
        flagCounter++;
    }
    cr->restore();
}

void
GameArea::displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &cr,
                               int xCoordinate,
                               int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, someImg, xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  someImg->get_width(), someImg->get_height());
    cr->fill();
    cr->restore();
}

void GameArea::setResources(PlayersMonitor *playersMonitor,
                            BuildingsMonitor *buildingsMonitor,
                            MapMonitor *mapMonitor) {
    this->playersMonitor = playersMonitor;
    this->buildingsMonitor = buildingsMonitor;
    this->mapMonitor = mapMonitor;
    this->camera.setMapWidth(mapMonitor->getXSize());
    this->camera.setMapHeight(mapMonitor->getYSize());
}

void GameArea::keyboardPressed() {
    std::cout<<"sth pressed"<<std::endl;
}

bool GameArea::on_key_press_event(GdkEventKey *event) {
    if (event->keyval == GDK_KEY_Up and event->keyval == GDK_KEY_Left) {
        camera.moveUp();
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Down){
        camera.moveDown();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Left){
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Right){
        camera.moveRight();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_Up) {
        camera.moveUp();
//        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    }else if (event->keyval == GDK_KEY_0){
        std::cout<<"clicked"<<std::endl;
        //returning true, cancels the propagation of the event
        return true;
    }
//
//    //if the event has not been handled, call the base class
//    return Gtk::Window::on_key_press_event(key_event);
}

bool GameArea::on_button_press_event(GdkEventButton *event) {
    /** From https://developer.gnome.org/gdk3/stable/gdk3-Event
     *                          -Structures.html#GdkEventButton
     *
     * GdkEventType type;   the type of the event (GDK_BUTTON_PRESS,
     *                      GDK_2BUTTON_PRESS, GDK_3BUTTON_PRESS or
     *                      GDK_BUTTON_RELEASE).
     *
     * GdkWindow *window;   the window which received the event.
     *
     * gint8 send_event;    TRUE if the event was sent explicitly.
     *
     * guint32 time;        the time of the event in milliseconds.
     *
     * gdouble x;           the x coordinate of the pointer relative to the
     *                      window.
     *
     * gdouble y;           the y coordinate of the pointer relative to the
     *                      window.
     * gdouble *axes;       x , y translated to the axes of device , or NULL
     *                      if device is the mouse.
     *
     * guint state;         a bit-mask representing the state of the modifier
     *                      keys (e.g. Control, Shift and Alt) and the pointer
     *                      buttons. See GdkModifierType.
     *
     * guint button;        the button which was pressed or released, numbered
     *                      from 1 to 5. Normally button 1 is the left mouse
     *                      button, 2 is the middle button, and 3 is the right
     *                      button. On 2-button mice, the middle button can
     *                      often be simulated by pressing both
     *                      mouse buttons together.
     *
     * GdkDevice *device;   the master device that the event originated from.
     *                      Use gdk_event_get_source_device()
     *                      to get the slave device.
     *
     * gdouble x_root;      the x coordinate of the pointer relative to
     *                      the root of the screen.
     *
     * gdouble y_root;      the y coordinate of the pointer relative to
     *                      the root of the screen.
     *
     */
    if (event->button == 1) {
        gdouble x = event->x;
        gdouble y = event->y;
        std::cout<<"X: "<<x<<", y: "<<y<<std::endl;
        //returning true, cancels the propagation of the event
        return true;
    }
}
