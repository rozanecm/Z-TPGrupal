#include <gtkmm/builder.h>
#include <gdkmm.h>
#include <iostream>
#include "GameArea.h"
#include <giomm.h>

#define TILESIZE 16    //tile width in pixels.
#define NUMBER_OF_TILES_TO_SHOW 10

GameArea::GameArea(BaseObjectType *cobject,
                   const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::DrawingArea(cobject),
        unitsMonitor(nullptr),
        buildingsMonitor(nullptr),
        mapMonitor(nullptr),
        unitsSelected(false),
        buildingSelected(false),
        coords({-1, -1}),
        /* camera is initialized with size 0,0 because we don't
         * have this data yet */
        camera(TILESIZE, 0, 0, NUMBER_OF_TILES_TO_SHOW) {
    loadResources();
    initializeCounters();

    add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    add_events(Gdk::EventMask::BUTTON_RELEASE_MASK);
    add_events(Gdk::EventMask::KEY_PRESS_MASK);
    set_can_focus(true);
}

void GameArea::loadResources() {
    try {
        /* load flags animations */
        loadFlagAnimations();
        /* load units resources */
        loadUnitsResources();
        /* load buildings resources */
        loadBuildingsResources();

        /* load some img */
        someImg = Gdk::Pixbuf::create_from_file("res/portraits/grunt.png");

        /* Load tiles */
        tiles["Tierra"] = Gdk::Pixbuf::create_from_file(
                "res/assets/tiles/tierra.png");
        tiles["Agua"] = Gdk::Pixbuf::create_from_file
                ("res/assets/tiles/agua.png");
        tiles["Lava"] = Gdk::Pixbuf::create_from_file(
                "res/assets/tiles/lava.png");
    } catch (Glib::FileError e) {
        std::cerr << e.what();
    }
    std::cerr << "resources succesfully loaded" << std::endl;
}

GameArea::~GameArea() {}

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    drawBaseMap(cr, camera.getPosition());
    drawBuildingsInView(cr);
    drawUnitsInMap(cr);
    updateCounters();
    return true;
}

void GameArea::drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr,
                           std::pair<unsigned int, unsigned int>
                           cameraPosition) {
    /* cameraPosition is given in pixels.
     * i,j indicate TILES. */
    if (mapMonitor->getXSize() == 0 and mapMonitor->getYSize() == 0) {
        return;
    }

    for (unsigned int i = 0; i < NUMBER_OF_TILES_TO_SHOW; ++i) {
        for (unsigned int j = 0; j < NUMBER_OF_TILES_TO_SHOW; ++j) {
            drawTileAt(cr, i, j, mapMonitor->getTerrainTypeAt(
                    cameraPosition.first / TILESIZE -
                    NUMBER_OF_TILES_TO_SHOW / 2 + i,
                    cameraPosition.second / TILESIZE -
                    NUMBER_OF_TILES_TO_SHOW / 2 + j));
        }
    }
}

void GameArea::drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                          unsigned int xTileCoordinate,
                          unsigned int yTileCoordinate,
                          std::string terrainType) {
    cr->save();
    auto pixbuf = tiles.find(terrainType);
    if (pixbuf == tiles.end()) {
        return;
    }
    const unsigned int xGraphicCoordinate = xTileCoordinate * get_width() /
                                            NUMBER_OF_TILES_TO_SHOW;
    const unsigned int yGraphicCoordinate = yTileCoordinate * get_height() /
                                            NUMBER_OF_TILES_TO_SHOW;
    Gdk::Cairo::set_source_pixbuf(cr, pixbuf->second,
                                  xGraphicCoordinate,
                                  yGraphicCoordinate);

    cr->rectangle(xGraphicCoordinate, yGraphicCoordinate,
                  get_width() / NUMBER_OF_TILES_TO_SHOW,
                  get_height() / NUMBER_OF_TILES_TO_SHOW);
    cr->fill();
    cr->restore();
}

void GameArea::drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &cr,
                                 int xGraphicCoordinate,
                                 int yGraphicCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, flags.at(TeamEnum::BLUE).
                                          at(flagCounter.getCounter()),
                                  xGraphicCoordinate, yGraphicCoordinate);

    cr->rectangle(xGraphicCoordinate, yGraphicCoordinate,
                  flags.at(TeamEnum::BLUE).
                          at(flagCounter.getCounter())->get_width(),
                  flags.at(TeamEnum::BLUE).
                          at(flagCounter.getCounter())->get_height());
    cr->fill();
    cr->restore();
}

void GameArea::drawBuildingsInView(const Cairo::RefPtr<Cairo::Context> &cr) {
    /* pointers (Unit*) are not used here because we are working with a shared
     * resource. This way, we copy the units we want to draw in a protected way,
     * and then we can draw without blocking other code. */
    std::vector<Building> buildingsToDraw =
            buildingsMonitor->getBuildingsToDraw(
                    camera.getPosition().first - (NUMBER_OF_TILES_TO_SHOW *
                                                  TILESIZE) / 2,
                    camera.getPosition().first + (NUMBER_OF_TILES_TO_SHOW *
                                                  TILESIZE) / 2,
                    camera.getPosition().second - (NUMBER_OF_TILES_TO_SHOW *
                                                   TILESIZE) / 2,
                    camera.getPosition().second + (NUMBER_OF_TILES_TO_SHOW *
                                                   TILESIZE) / 2);

    for (auto &building : buildingsToDraw) {
        /* call actual drawing method */
        drawBuilding(building.getBuildingType(), buildingsCounter.getCounter(),
                     building.getTeam(), cr,
                     cameraToRealMapX(camera.idealMapToCameraXCoordinate
                             (building.getXCoordinate())),
                     cameraToRealMapY(camera.idealMapToCameraYCoordinate
                             (building.getYCoordinate())));
    }
}

void GameArea::drawBuilding(BuildingsEnum buildingType, unsigned short counter,
                            TeamEnum team,
                            const Cairo::RefPtr<Cairo::Context> &cr,
                            unsigned int xGraphicCoordinate,
                            unsigned int yGraphicCoordinate) {
    cr->save();
    Glib::RefPtr<Gdk::Pixbuf> next = buildings.at(buildingType).at(counter);

    /* perform actual drawing */
    Gdk::Cairo::set_source_pixbuf(cr, next,
                                  xGraphicCoordinate, yGraphicCoordinate);

    cr->rectangle(xGraphicCoordinate, yGraphicCoordinate, next->get_width(),
                  next->get_height());

    cr->fill();
    cr->restore();
    /* draw flag */
    drawFlag(team, cr, xGraphicCoordinate, yGraphicCoordinate);
}

void GameArea::drawFlag(const TeamEnum &team,
                        const Cairo::RefPtr<Cairo::Context> &cr,
                        unsigned int xGraphicCoordinate,
                        unsigned int yGraphicCoordinate) const {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr,
                                  flags.at(team).at(flagCounter.getCounter()),
                                  xGraphicCoordinate, yGraphicCoordinate);

    cr->rectangle(xGraphicCoordinate,
                  yGraphicCoordinate,
                  flags.at(team).at(flagCounter.getCounter())->get_width(),
                  flags.at(team).at(flagCounter.getCounter())->get_height());
    cr->fill();
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

void GameArea::setResources(UnitsMonitor *unitsMonitor,
                            BuildingsMonitor *buildingsMonitor,
                            MapMonitor *mapMonitor) {
    this->unitsMonitor = unitsMonitor;
    this->buildingsMonitor = buildingsMonitor;
    this->mapMonitor = mapMonitor;
}

bool GameArea::on_key_press_event(GdkEventKey *event) {
    if (event->keyval == GDK_KEY_Up and event->keyval == GDK_KEY_Left) {
        camera.moveUp();
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Down) {
        camera.moveDown();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Left) {
        camera.moveLeft();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Right) {
        camera.moveRight();
        //returning true, cancels the propagation of the event
        return true;
    } else if (event->keyval == GDK_KEY_Up) {
        camera.moveUp();
        //returning true, cancels the propagation of the event
        return true;
    }
//  todo ver si el event handling se pasa arriba o no
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
     *
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
        unitsMonitor->wipeSelected();
        buildingsMonitor->wipe_selected();
        unitsSelected = false;
        buildingSelected = false;
        xStartCoordinate = event->x;
        yStartCoordinate = event->y;
        /* returning true, cancels the propagation of the event */
    }
    return true;

}

bool GameArea::on_button_release_event(GdkEventButton *event) {
    if (event->button == 1) {
        xFinishCoordinate = event->x;
        yFinishCoordinate = event->y;
        makeSelection();
        coords = {camera.cameraToMapXCoordinate(realMapToCamera(event->x)),
                  camera.cameraToMapYCoordinate(realMapToCamera(event->y))};
        /* returning true, cancels the propagation of the event. We return
         * false, so the event can be handled by the game window
         * */
    }
    return false;
}

void GameArea::makeSelection() {
    /* tell each of the structures storing objects in the map to mark as
     * selected the items which are within the mouse selection */
    //todo filter out other players' units.
    unitsMonitor->markAsSelectedInRange(unitsSelected,
            camera.cameraToMapXCoordinate(realMapToCamera(xStartCoordinate)),
            camera.cameraToMapYCoordinate(realMapToCamera(yStartCoordinate)),
            camera.cameraToMapXCoordinate(realMapToCamera(xFinishCoordinate)),
            camera.cameraToMapYCoordinate(realMapToCamera(yFinishCoordinate)));
    if (!unitsSelected) {
        buildingsMonitor->markAsSelectedInRange(
                buildingSelected,
                camera.cameraToMapXCoordinate(
                        realMapToCamera(xStartCoordinate)),
                camera.cameraToMapYCoordinate(
                        realMapToCamera(yStartCoordinate)),
                camera.cameraToMapXCoordinate(
                        realMapToCamera(xFinishCoordinate)),
                camera.cameraToMapYCoordinate(
                        realMapToCamera(yFinishCoordinate)));
    } else {
        mapMonitor->markAsSelectedInRange(
                terrainSelected,
                camera.cameraToMapXCoordinate(
                        realMapToCamera(xStartCoordinate)),
                camera.cameraToMapYCoordinate(
                        realMapToCamera(yStartCoordinate)),
                camera.cameraToMapXCoordinate(
                        realMapToCamera(xFinishCoordinate)),
                camera.cameraToMapYCoordinate(
                        realMapToCamera(yFinishCoordinate)));
    }
    selectionMade = false;
}


void GameArea::processClick() {
    //todo complete method processClick
    if (unitsSelected) {
    }
}

void GameArea::loadUnitsResources() {
    /* load fire animations */
    loadGruntFireAnimations();
    loadLaserFireAnimations();
    loadPsychoFireAnimations();
    loadPyroFireAnimations();
    loadSniperFireAnimations();
    loadToughFireAnimations();

    /* load walking animations */
    loadBlueWalkingAnimations();
    loadGreenWalkingAnimations();
    loadRedWalkingAnimations();
    loadYellowWalkingAnimations();

    /* load standing animations */
    loadBlueStandingAnimations();
    loadGreenStandingAnimations();
    loadRedStandingAnimations();
    loadYellowStandingAnimations();

    /* load vehicles' animations */
    loadTiresAnimations();
    loadNeuterVehiclesAnimations();
    loadBlueVehiclesAnimations();
    loadGreenVehiclesAnimations();
    loadRedVehiclesAnimations();
    loadYellowVehiclesAnimations();
}

/* FLAGS animations loading */
void GameArea::loadFlagAnimations() {
    /* this methods loads all the imgs needed to draw all the flags' animations.
     * POSSIBLE OPTIMIZATION: load only needed colors */
    flags[TeamEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n00.png"));

    flags[TeamEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n01.png"));

    flags[TeamEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n02.png"));

    flags[TeamEnum::BLUE].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_blue_n03.png"));

    /* load red flags imgs */
    flags[TeamEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n00.png"));

    flags[TeamEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n01.png"));

    flags[TeamEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n02.png"));

    flags[TeamEnum::RED].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_red_n03.png"));

    /* load yellow flags imgs */
    flags[TeamEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n00.png"));

    flags[TeamEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n01.png"));

    flags[TeamEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n02.png"));

    flags[TeamEnum::YELLOW].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_yellow_n03.png"));

    /* load green flags imgs */
    flags[TeamEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n00.png"));

    flags[TeamEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n01.png"));

    flags[TeamEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n02.png"));

    flags[TeamEnum::GREEN].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_green_n03.png"));

    /* load neuter flags imgs */
    flags[TeamEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n00.png"));

    flags[TeamEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n01.png"));

    flags[TeamEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n02.png"));

    flags[TeamEnum::NEUTER].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/flags/flag_null_n03.png"));
}

/* FIRE animations loading */
void GameArea::loadGruntFireAnimations() {
    loadBlueGruntFireAnimations();
    loadGreenGruntFireAnimations();
    loadRedGruntFireAnimations();
    loadYellowGruntFireAnimations();
}

void GameArea::loadBlueGruntFireAnimations() {
    /* blue grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r000_n04.png"));

    /* blue grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r045_n04.png"));

    /* blue grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r090_n04.png"));

    /* blue grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r135_n04.png"));

    /* blue grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r180_n04.png"));

    /* blue grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r225_n04.png"));

    /* blue grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r270_n04.png"));

    /* blue grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_blue_r315_n04.png"));
}

void GameArea::loadGreenGruntFireAnimations() {
    /* green grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r000_n04.png"));

    /* green grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r045_n04.png"));

    /* green grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r090_n04.png"));

    /* green grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r135_n04.png"));

    /* green grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r180_n04.png"));

    /* green grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r225_n04.png"));

    /* green grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r270_n04.png"));

    /* green grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_green_r315_n04.png"));
}

void GameArea::loadRedGruntFireAnimations() {
    /* red grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r000_n04.png"));

    /* red grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r045_n04.png"));

    /* red grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r090_n04.png"));

    /* red grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/grunt/fire_red_r135_n04.png"));

    /* red grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r180_n04.png"));

    /* red grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r225_n04.png"));

    /* red grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r270_n04.png"));

    /* red grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_red_r315_n04.png"));
}

void GameArea::loadYellowGruntFireAnimations() {
    /* yellow grunt fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r000_n04.png"));

    /* yellow grunt fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r045_n04.png"));

    /* yellow grunt fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r090_n04.png"));

    /* yellow grunt fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r135_n04.png"));

    /* yellow grunt fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r180_n04.png"));

    /* yellow grunt fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r225_n04.png"));

    /* yellow grunt fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r270_n04.png"));

    /* yellow grunt fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::GRUNT][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/grunt/fire_yellow_r315_n04.png"));
}

void GameArea::loadLaserFireAnimations() {
    loadBlueLaserFireAnimations();
    loadGreenLaserFireAnimations();
    loadRedLaserFireAnimations();
    loadYellowLaserFireAnimations();
}

void GameArea::loadBlueLaserFireAnimations() {
    /* blue laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r000_n02.png"));

    /* blue laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r045_n02.png"));

    /* blue laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r090_n02.png"));

    /* blue laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r135_n02.png"));

    /* blue laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r180_n02.png"));

    /* blue laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r225_n02.png"));

    /* blue laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r270_n02.png"));

    /* blue laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenLaserFireAnimations() {
    /* green laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r000_n02.png"));

    /* green laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r045_n02.png"));

    /* green laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r090_n02.png"));

    /* green laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r135_n02.png"));

    /* green laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r180_n02.png"));

    /* green laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r225_n02.png"));

    /* green laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r270_n02.png"));

    /* green laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_green_r315_n02.png"));
}

void GameArea::loadRedLaserFireAnimations() {
    /* red laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r000_n02.png"));

    /* red laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r045_n02.png"));

    /* red laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r090_n02.png"));

    /* red laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r135_n02.png"));

    /* red laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r180_n02.png"));

    /* red laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r225_n02.png"));

    /* red laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r270_n02.png"));

    /* red laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_red_r315_n02.png"));
}

void GameArea::loadYellowLaserFireAnimations() {
    /* yellow laser fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r000_n02.png"));

    /* yellow laser fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r045_n02.png"));

    /* yellow laser fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r090_n02.png"));

    /* yellow laser fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r135_n02.png"));

    /* yellow laser fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r180_n02.png"));

    /* yellow laser fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r225_n02.png"));

    /* yellow laser fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r270_n02.png"));

    /* yellow laser fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::LASER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/laser/fire_yellow_r315_n02.png"));
}

void GameArea::loadPsychoFireAnimations() {
    loadBluePsychoFireAnimations();
    loadGreenPsychoFireAnimations();
    loadRedPsychoFireAnimations();
    loadYellowPsychoFireAnimations();
}

void GameArea::loadBluePsychoFireAnimations() {
    /* blue psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r000_n01.png"));

    /* blue psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r045_n01.png"));

    /* blue psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r090_n01.png"));

    /* blue psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r135_n01.png"));

    /* blue psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r180_n01.png"));

    /* blue psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r225_n01.png"));

    /* blue psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r270_n01.png"));

    /* blue psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_blue_r315_n01.png"));
}

void GameArea::loadGreenPsychoFireAnimations() {
    /* green psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r000_n01.png"));

    /* green psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r045_n01.png"));

    /* green psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r090_n01.png"));

    /* green psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r135_n01.png"));

    /* green psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r180_n01.png"));

    /* green psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r225_n01.png"));

    /* green psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r270_n01.png"));

    /* green psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_green_r315_n01.png"));
}

void GameArea::loadRedPsychoFireAnimations() {
    /* red psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r000_n01.png"));

    /* red psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r045_n01.png"));

    /* red psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r090_n01.png"));

    /* red psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r135_n01.png"));

    /* red psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r180_n01.png"));

    /* red psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r225_n01.png"));

    /* red psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r270_n01.png"));

    /* red psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_red_r315_n01.png"));
}

void GameArea::loadYellowPsychoFireAnimations() {
    /* yellow psycho fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r000_n01.png"));

    /* yellow psycho fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r045_n01.png"));

    /* yellow psycho fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r090_n01.png"));

    /* yellow psycho fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r135_n01.png"));

    /* yellow psycho fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r180_n01.png"));

    /* yellow psycho fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r225_n01.png"));

    /* yellow psycho fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r270_n01.png"));

    /* yellow psycho fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PSYCHO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/psycho/fire_yellow_r315_n01.png"));
}

void GameArea::loadPyroFireAnimations() {
    loadBluePyroFireAnimation();
    loadRedPyroFireAnimation();
    loadGreenPyroFireAnimation();
    loadYellowPyroFireAnimation();
}

void GameArea::loadBluePyroFireAnimation() {
    /* blue pyro fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r000_n02.png"));

    /* blue pyro fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r045_n02.png"));

    /* blue pyro fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r090_n02.png"));

    /* blue pyro fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r135_n02.png"));

    /* blue pyro fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r180_n02.png"));

    /* blue pyro fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r225_n02.png"));

    /* blue pyro fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r270_n02.png"));

    /* blue pyro fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenPyroFireAnimation() {
    /* green pyro fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r000_n02.png"));

    /* green pyro fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r045_n02.png"));

    /* green pyro fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r090_n02.png"));

    /* green pyro fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r135_n02.png"));

    /* green pyro fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r180_n02.png"));

    /* green pyro fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r225_n02.png"));

    /* green pyro fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r270_n02.png"));

    /* green pyro fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_green_r315_n02.png"));
}

void GameArea::loadRedPyroFireAnimation() {
    /* red pyro fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r000_n02.png"));

    /* red pyro fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r045_n02.png"));

    /* red pyro fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r090_n02.png"));

    /* red pyro fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r135_n02.png"));

    /* red pyro fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r180_n02.png"));

    /* red pyro fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r225_n02.png"));

    /* red pyro fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r270_n02.png"));

    /* red pyro fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_red_r315_n02.png"));
}

void GameArea::loadYellowPyroFireAnimation() {
    /* yellow pyro fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r000_n02.png"));

    /* yellow pyro fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r045_n02.png"));

    /* yellow pyro fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r090_n02.png"));

    /* yellow pyro fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r135_n02.png"));

    /* yellow pyro fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r180_n02.png"));

    /* yellow pyro fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r225_n02.png"));

    /* yellow pyro fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r270_n02.png"));

    /* yellow pyro fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::PYRO][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/pyro/fire_yellow_r315_n02.png"));
}

void GameArea::loadSniperFireAnimations() {
    loadBlueSniperFireAnimations();
    loadGreenSniperFireAnimations();
    loadRedSniperFireAnimations();
    loadYellowSniperFireAnimations();
}

void GameArea::loadBlueSniperFireAnimations() {
    /* blue sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r000_n04.png"));

    /* blue sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r045_n04.png"));

    /* blue sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r090_n04.png"));

    /* blue sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r135_n04.png"));

    /* blue sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r180_n04.png"));

    /* blue sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r225_n04.png"));

    /* blue sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r270_n04.png"));

    /* blue sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_blue_r315_n04.png"));
}

void GameArea::loadGreenSniperFireAnimations() {
    /* green sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r000_n04.png"));

    /* green sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r045_n04.png"));

    /* green sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r090_n04.png"));

    /* green sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r135_n04.png"));

    /* green sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r180_n04.png"));

    /* green sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r225_n04.png"));

    /* green sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r270_n04.png"));

    /* green sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_green_r315_n04.png"));
}

void GameArea::loadRedSniperFireAnimations() {
    /* red sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r000_n04.png"));

    /* red sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r045_n04.png"));

    /* red sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r090_n04.png"));

    /* red sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r135_n04.png"));

    /* red sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r180_n04.png"));

    /* red sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r225_n04.png"));

    /* red sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r270_n04.png"));

    /* red sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_red_r315_n04.png"));
}

void GameArea::loadYellowSniperFireAnimations() {
    /* yellow sniper fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r000_n04.png"));

    /* yellow sniper fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r045_n04.png"));

    /* yellow sniper fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r090_n04.png"));

    /* yellow sniper fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r135_n04.png"));

    /* yellow sniper fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r180_n04.png"));

    /* yellow sniper fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r225_n04.png"));

    /* yellow sniper fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r270_n04.png"));

    /* yellow sniper fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n02.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n03.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::SNIPER][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/sniper/fire_yellow_r315_n04.png"));
}

void GameArea::loadToughFireAnimations() {
    loadBlueToughFireAnimations();
    loadGreenToughFireAnimations();
    loadRedToughFireAnimations();
    loadYellowToughFireAnimations();
}

void GameArea::loadBlueToughFireAnimations() {
    /* blue tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r000_n02.png"));

    /* blue tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r045_n02.png"));

    /* blue tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r090_n02.png"));

    /* blue tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r135_n02.png"));

    /* blue tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r180_n02.png"));

    /* blue tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r225_n02.png"));

    /* blue tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r270_n02.png"));

    /* blue tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::BLUE)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_blue_r315_n02.png"));
}

void GameArea::loadGreenToughFireAnimations() {
    /* green tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r000_n02.png"));

    /* green tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r045_n02.png"));

    /* green tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r090_n02.png"));

    /* green tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r135_n02.png"));

    /* green tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r180_n02.png"));

    /* green tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r225_n02.png"));

    /* green tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r270_n02.png"));

    /* green tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::GREEN)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_green_r315_n02.png"));
}

void GameArea::loadRedToughFireAnimations() {
    /* red tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r000_n02.png"));

    /* red tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r045_n02.png"));

    /* red tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r090_n02.png"));

    /* red tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r135_n02.png"));

    /* red tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r180_n02.png"));

    /* red tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r225_n02.png"));

    /* red tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r270_n02.png"));

    /* red tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::RED)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_red_r315_n02.png"));
}

void GameArea::loadYellowToughFireAnimations() {
    /* yellow tough fire at 0 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r000_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r000_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r000].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r000_n02.png"));

    /* yellow tough fire at 45 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r045_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r045_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r045].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r045_n02.png"));

    /* yellow tough fire at 90 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r090_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r090_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r090].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r090_n02.png"));

    /* yellow tough fire at 135 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r135_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r135_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r135].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r135_n02.png"));

    /* yellow tough fire at 180 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r180_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r180_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r180].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r180_n02.png"));

    /* yellow tough fire at 225 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r225_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r225_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r225].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r225_n02.png"));

    /* yellow tough fire at 270 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r270_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r270_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r270].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r270_n02.png"));

    /* yellow tough fire at 315 degrees */
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r315_n00.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r315_n01.png"));
    unitsAnimations.operator[](TeamEnum::YELLOW)[UnitsEnum::TOUGH][
            ActionsEnum::FIRE][RotationsEnum::r315].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/units/robots/tough/fire_yellow_r315_n02.png"));
}

/* MOVE animations loading */
void GameArea::loadBlueWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_blue_r315_n03.png"));
}

void GameArea::loadGreenWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_green_r315_n03.png"));
}

void GameArea::loadRedWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_red_r315_n03.png"));
}

void GameArea::loadYellowWalkingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r000_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r045_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r090_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r135_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r180_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r225_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r270_n03.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n02.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/walk_yellow_r315_n03.png"));
}

/* STAND animations loading */
void GameArea::loadBlueStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_blue_r315.png"));
}

void GameArea::loadGreenStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_green_r315.png"));
}

void GameArea::loadRedStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_red_r315.png"));
}

void GameArea::loadYellowStandingAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::GENERIC_ROBOT][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/robots/stand_yellow_r315.png"));
}

/* BUILDINGS' animations loading */
void GameArea::loadBuildingsResources() {
    buildings[BuildingsEnum::FORT].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/fort_jungle_front.png"));

    buildings[BuildingsEnum::FORT_DESTROYED].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/buildings/fort/fort_jungle_front_destroyed.png"));

    buildings[BuildingsEnum::VEHICLE_FABRIC].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/buildings/vehicle/base_jungle.png"));

    buildings[BuildingsEnum::VEHICLE_FABRIC_DESTROYED].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/buildings/vehicle/base_destroyed_jungle.png"));

    buildings[BuildingsEnum::ROBOT_FABRIC].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/buildings/robot/base_jungle.png"));

    buildings[BuildingsEnum::ROBOT_FABRI_DESTROYEDC].emplace_back(
            Gdk::Pixbuf::create_from_file(
                    "res/assets/buildings/robot/base_destroyed_jungle.png"));
}

/* VEHICLES' animations loading */
void GameArea::loadTiresAnimations() {
    loadJeepTires();
}

void GameArea::loadJeepTires() {
    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n00.png"));

    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n01.png"));

    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n02.png"));

    jeepTires[RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r000_n03.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n00.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n01.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n02.png"));

    jeepTires[RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r045_n03.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n00.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n01.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n02.png"));

    jeepTires[RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r135_n03.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n00.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n01.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n02.png"));

    jeepTires[RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r180_n03.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n00.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n01.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n02.png"));

    jeepTires[RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r225_n03.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n00.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n01.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n02.png"));

    jeepTires[RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/under_r315_n03.png"));
}

void GameArea::loadNeuterVehiclesAnimations() {
    loadNeuterJeepAnimations();
}

void GameArea::loadNeuterJeepAnimations() {
    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r000.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r045.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r090.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r135.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r180.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r225.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r270.png"));

    unitsAnimations.operator[](
            TeamEnum::NEUTER)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/empty_r315.png"));
}

void GameArea::loadBlueVehiclesAnimations() {
    loadBlueJeepAnimations();
    loadBlueLightTankAnimations();
    loadBlueMediumTankAnimations();
    loadBlueMMLAnimations();
    loadBlueHeavyTankAnimations();
}

void GameArea::loadBlueJeepAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_blue_r315_n01.png"));
}

void GameArea::loadBlueLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_blue_r315_n02.png"));
}

void GameArea::loadBlueMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_blue_r315_n02.png"));
}

void GameArea::loadBlueMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_blue_r315_n02.png"));
}

void GameArea::loadBlueHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::BLUE)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_blue_r315_n02.png"));
}

void GameArea::loadGreenVehiclesAnimations() {
    loadGreenJeepAnimations();
    loadGreenLightTankAnimations();
    loadGreenMediumTankAnimations();
    loadGreenMMLAnimations();
    loadGreenHeavyTankAnimations();
}

void GameArea::loadGreenJeepAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_green_r315_n01.png"));
}

void GameArea::loadGreenLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_green_r315_n02.png"));
}

void GameArea::loadGreenMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_green_r315_n02.png"));
}

void GameArea::loadGreenMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_green_r315_n02.png"));
}

void GameArea::loadGreenHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::GREEN)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_green_r315_n02.png"));
}

void GameArea::loadRedVehiclesAnimations() {
    loadRedJeepAnimations();
    loadRedLightTankAnimations();
    loadRedMediumTankAnimations();
    loadRedMMLAnimations();
    loadRedHeavyTankAnimations();
}

void GameArea::loadRedJeepAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_red_r315_n01.png"));
}

void GameArea::loadRedLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_red_r315_n02.png"));
}

void GameArea::loadRedMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_red_r315_n02.png"));
}

void GameArea::loadRedMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_red_r315_n02.png"));
}

void GameArea::loadRedHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::RED)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_red_r315_n02.png"));
}

void GameArea::loadYellowVehiclesAnimations() {
    loadYellowJeepAnimations();
    loadYellowLightTankAnimations();
    loadYellowMediumTankAnimations();
    loadYellowMMLAnimations();
    loadYellowHeavyTankAnimations();
}

void GameArea::loadYellowJeepAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r000].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r045].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r090].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r135_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r135].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r135_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r180_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r180].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r180_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r225_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r225].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r225_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r270_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r270].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r270_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::JEEP][ActionsEnum::STAND]
    [RotationsEnum::r315].emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/jeep/base_yellow_r315_n01.png"));
}

void GameArea::loadYellowLightTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/light_tank/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowMediumTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MEDIUM_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/medium_tank/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowMMLAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n00.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n01.png"));
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::MML][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/mml/base_yellow_r315_n02.png"));
}

void GameArea::loadYellowHeavyTankAnimations() {
    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r000].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r000_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r045].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r045_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r090].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r090_n02.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n00.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n01.png"));

    unitsAnimations.operator[](
            TeamEnum::YELLOW)[UnitsEnum::HEAVY_TANK][ActionsEnum::STAND]
    [RotationsEnum::r315].
            emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/units/vehicles/heavy_tank/base_yellow_r315_n02.png"));
}

void GameArea::drawJeepTires(const Cairo::RefPtr<Cairo::Context> &cr,
                             unsigned int xGraphicCoordinate,
                             unsigned int yGraphicCoordinate,
                             RotationsEnum rotation) {
    cr->save();
    /* first draw jeepTires */
    Gdk::Cairo::set_source_pixbuf(cr, jeepTires.at(rotation).
                                          at(tireCounter.getCounter()),
                                  xGraphicCoordinate, yGraphicCoordinate);
    cr->rectangle(xGraphicCoordinate, yGraphicCoordinate,
                  jeepTires.at(rotation).
                          at(tireCounter.getCounter())->get_width(),
                  jeepTires.at(rotation).
                          at(tireCounter.getCounter())->get_height());
    cr->fill();

    cr->restore();
}

void GameArea::drawUnit(TeamEnum team, UnitsEnum unitType,
                        ActionsEnum actionType,
                        RotationsEnum rotation, unsigned short unitCounter,
                        const Cairo::RefPtr<Cairo::Context> &cr,
                        unsigned int xGraphicCoordinate,
                        unsigned int yGraphicCoordinate) {
    cr->save();
    /* adapt given data to saved imgs. Applies to vehicles */
    if (unitType == UnitsEnum::JEEP &&
        rotation != RotationsEnum::r090 &&
        rotation != RotationsEnum::r270) {
        /* rotations 090 and 270 dont have tires */
        drawJeepTires(cr, xGraphicCoordinate, yGraphicCoordinate, rotation);
    }
    processUnitToDrawEnums(unitType, actionType, rotation);

    auto team_map = unitsAnimations.find(team);
    if (team_map == unitsAnimations.end()) {
        std::cerr << "Drawing failed at finding valid team" << std::endl;
    }

    auto unit_map = team_map->second.find(unitType);
    if (unit_map == team_map->second.end()) {
        std::cerr << "Drawing failed at finding valid unitType" << std::endl;
    }

    auto actions_map = unit_map->second.find(actionType);
    if (actions_map == unit_map->second.end()) {
        std::cerr << "Drawing failed at finding valid actionType" << std::endl;
    }

    auto rotations_map = actions_map->second.find(rotation);
    if (rotations_map == actions_map->second.end()) {
        std::cerr << "Drawing failed at finding valid rotation" << std::endl;
    }

//    if (unitIsRobot(unitType)){
//        cr->scale(1.5, 1.5);
//    }

    auto next = rotations_map->second.at(unitCounter);
    /* perform actual drawing */
    Gdk::Cairo::set_source_pixbuf(cr, next,
                                  xGraphicCoordinate, yGraphicCoordinate);

    cr->rectangle(xGraphicCoordinate, yGraphicCoordinate, next->get_width(),
                  next->get_height());
    cr->fill();
    cr->restore();
}

void
GameArea::processUnitToDrawEnums(UnitsEnum &unitType, ActionsEnum &actionType,
                                 RotationsEnum &rotation) const {
    if (unitType == UnitsEnum::HEAVY_TANK
        or unitType == UnitsEnum::LIGHT_TANK
        or unitType == UnitsEnum::MEDIUM_TANK
        or unitType == UnitsEnum::MML) {
        actionType = ActionsEnum::STAND;
        /* same assets are used for given rotations;
         * e.g.: 135 and 315 are drawn with same img */
        if (rotation == RotationsEnum::r135) {
            rotation = RotationsEnum::r315;
        } else if (rotation == RotationsEnum::r180) {
            rotation = RotationsEnum::r000;
        } else if (rotation == RotationsEnum::r225) {
            rotation = RotationsEnum::r045;
        } else if (rotation == RotationsEnum::r270) {
            rotation = RotationsEnum::r090;
        }
    } else if ((unitType == UnitsEnum::GRUNT
                or unitType == UnitsEnum::LASER
                or unitType == UnitsEnum::PSYCHO
                or unitType == UnitsEnum::PYRO
                or unitType == UnitsEnum::SNIPER
                or unitType == UnitsEnum::TOUGH)
               and (actionType == ActionsEnum::MOVE
                    or actionType == ActionsEnum::STAND)) {
        /* because same imgs are used to draw all different types of robots
         * when these are moving or standing still, if this is the case, we
         * set the unit type to generic robot */
        unitType = UnitsEnum::GENERIC_ROBOT;
    }
}

void GameArea::drawUnitsInMap(const Cairo::RefPtr<Cairo::Context> &cr) {
    /* pointers (Unit*) are not used here because we are working with a shared
     * resource. This way, we copy the units we want to draw in a protected way,
     * and then we can draw without blocking other code. */
    std::vector<Unit> unitsToDraw = unitsMonitor->getUnitsToDraw(
            camera.getPosition().first -
            (NUMBER_OF_TILES_TO_SHOW * TILESIZE) / 2,
            camera.getPosition().first +
            (NUMBER_OF_TILES_TO_SHOW * TILESIZE) / 2,
            camera.getPosition().second - (NUMBER_OF_TILES_TO_SHOW *
                                           TILESIZE) / 2,
            camera.getPosition().second + (NUMBER_OF_TILES_TO_SHOW * TILESIZE)
                                          / 2);

    for (auto &unit : unitsToDraw) {
        /* check what is being drawn, and choose the counter appropriately. */
        unsigned short counter;
        counter = getCounter(unit);

        /* call actual drawing method */
        drawUnit(unit.getTeam(), unit.getType(), unit.getAction(),
                 unit.getRotation(),
                 counter, cr,
                 cameraToRealMapX(
                         camera.idealMapToCameraXCoordinate(
                                 unit.getXCoordinate())),
                 cameraToRealMapY(
                         camera.idealMapToCameraYCoordinate(
                                 unit.getYCoordinate())));
    }
}

unsigned short GameArea::getCounter(Unit &unit) const {
    if (unit.getType() == UnitsEnum::JEEP) {
        if (unit.getTeam() == TeamEnum::NEUTER){
            return 0;
        }
        return jeepCounter.getCounter();
    } else if (unit.getType() == UnitsEnum::LIGHT_TANK or
               unit.getType() == UnitsEnum::MEDIUM_TANK or
               unit.getType() == UnitsEnum::HEAVY_TANK) {
        return tankCounter.getCounter();
    } else if (unit.getAction() == ActionsEnum::FIRE) {
        return shootingRobotCounter.getCounter();
    } else if (unit.getAction() == ActionsEnum::MOVE) {
        return walkingRobotCounter.getCounter();
    } else {
        return standingRobotCounter.getCounter();
    }
}

void GameArea::updateCounters() {
    /* update units counters */
    flagCounter.updateCounter();

    shootingRobotCounter.updateCounter();

    walkingRobotCounter.updateCounter();

    standingRobotCounter.updateCounter();

    jeepCounter.updateCounter();

    tireCounter.updateCounter();

    tankCounter.updateCounter();

    mmlCounter.updateCounter();
    /* end update counter section */
}

void GameArea::initializeCounters() {
    /* one of the vectors of each category is accessed to get the size of the
     * vectors of all the category. This is possible because all vectors of
     * the same category share the same size */
    flagCounter.initialize(flags.at(TeamEnum::BLUE).size());

    jeepCounter.initialize(unitsAnimations.operator[](TeamEnum::BLUE)
                           [UnitsEnum::JEEP][ActionsEnum::STAND]
                           [RotationsEnum::r000].size());

    tireCounter.initialize(jeepTires.at(RotationsEnum::r000).size());

    standingRobotCounter.initialize(unitsAnimations.operator[](TeamEnum::BLUE)
                                    [UnitsEnum::GENERIC_ROBOT]
                                    [ActionsEnum::STAND]
                                    [RotationsEnum::r000].size());

    walkingRobotCounter.initialize(unitsAnimations.operator[](TeamEnum::BLUE)
                                   [UnitsEnum::GENERIC_ROBOT][ActionsEnum::MOVE]
                                   [RotationsEnum::r000].size());

    shootingRobotCounter.initialize(unitsAnimations.operator[](TeamEnum::BLUE)
                                    [UnitsEnum::PSYCHO][ActionsEnum::FIRE]
                                    [RotationsEnum::r000].size());

    tankCounter.initialize(unitsAnimations.operator[](TeamEnum::BLUE)
                           [UnitsEnum::LIGHT_TANK][ActionsEnum::STAND]
                           [RotationsEnum::r000].size());

    mmlCounter.initialize(unitsAnimations.operator[](TeamEnum::BLUE)
                          [UnitsEnum::HEAVY_TANK][ActionsEnum::STAND
                          ][RotationsEnum::r000].size());

    buildingsCounter.initialize(buildings.at(BuildingsEnum::FORT).size());
}

unsigned int GameArea::cameraToRealMapX(unsigned int coordinate) {
    return get_width() * coordinate / (NUMBER_OF_TILES_TO_SHOW * TILESIZE);
}

unsigned int GameArea::cameraToRealMapY(unsigned int coordinate) {
    return get_height() * coordinate / (NUMBER_OF_TILES_TO_SHOW * TILESIZE);
}

std::pair<int, int> GameArea::get_coords() {
    return coords;
}

unsigned int GameArea::realMapToCamera(gdouble coordinate) {
    return (NUMBER_OF_TILES_TO_SHOW * TILESIZE * coordinate) / (get_width());
}

void GameArea::setMapData() {
    this->camera.setMapWidth(mapMonitor->getXSize());
    this->camera.setMapHeight(mapMonitor->getYSize());
}

bool GameArea::unitIsRobot(UnitsEnum unitType) {
    return (unitType == UnitsEnum::GENERIC_ROBOT or
            unitType == UnitsEnum::GRUNT or unitType == UnitsEnum::LASER or
            unitType == UnitsEnum::PSYCHO or unitType == UnitsEnum::PYRO or
            unitType == UnitsEnum::SNIPER or unitType == UnitsEnum::TOUGH);
}

bool GameArea::unit_selected() {
    return unitsSelected;
}

bool GameArea::buildings_selected() {
    return buildingSelected;
}
