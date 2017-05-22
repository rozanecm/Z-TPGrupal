cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(src/sdl)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES
        src/main.cpp src/GameWindow.cpp src/GameWindow.h src/cell.cpp src/cell.h src/Terrain.cpp src/Terrain.h src/Occupant.cpp src/Occupant.h
        src/xml/pugixml.cpp src/xml/pugiconfig.hpp src/xml/pugixml.hpp src/MapLoader.cpp src/MapLoader.h src/compass.cpp src/compass.h src/lock.cpp src/lock.h src/lock.cpp src/lock.h src/node.cpp src/node.h src/size.cpp src/size.h src/map.cpp src/map.h src/unit.cpp src/unit.h src/position.cpp src/position.h src/weapon.cpp src/weapon.h src/toughType.cpp src/toughType.h)

add_executable(Z_TPGrupal ${SOURCE_FILES})


target_link_libraries(Z_TPGrupal
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARIES})