cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})


pkg_check_modules(SDL2 sdl2)
include_directories(${SDL2_LIBRARY_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})

pkg_check_modules(SDL2_mixer SDL2_mixer)
link_directories(${SDL2_mixer_LIBRARY_DIRS})
include_directories(${SDL2_mixer_INCLUDE_DIRS})

add_compile_options(${SDL2_mixer_LDFLAGS} ${SDL2_mixer_CFLAGS})

set(SOURCE_FILES
        src/main.cpp src/GameWindow.cpp src/GameWindow.h
        libs/xml/pugixml.cpp libs/xml/pugiconfig.hpp libs/xml/pugixml.hpp src/MapGenerator.cpp src/MapGenerator.h)

add_executable(Z_TPGrupal ${SOURCE_FILES})


target_link_libraries(Z_TPGrupal
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDL2_mixer_LIBRARIES})