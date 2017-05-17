cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

set(SDL2_INCLUDE_DIR  /usr/include/SDL2 )
set(SDL2_LIBRARY /usr/lib/libSDL2.so)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(/usr/local/include/SDL2/SDL2_mixer)

add_compile_options(-l SDL_mixer)

set(SOURCE_FILES
        src/main.cpp src/GameWindow.cpp src/GameWindow.h
        libs/xml/pugixml.cpp libs/xml/pugiconfig.hpp libs/xml/pugixml.hpp)

add_executable(Z_TPGrupal ${SOURCE_FILES})


target_link_libraries(Z_TPGrupal
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARY})