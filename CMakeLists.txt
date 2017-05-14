cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})
set(SOURCE_FILES
        src/main.cpp src/GameWindow.cpp src/GameWindow.h src/cell.cpp src/cell.h src/Terrain.cpp src/Terrain.h src/Occupant.cpp src/Occupant.h)

add_executable(Z_TPGrupal ${SOURCE_FILES})


target_link_libraries(Z_TPGrupal
        ${GTKMM_LIBRARIES}  )