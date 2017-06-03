cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)
set(CMAKE_CXX_STANDARD 11)

set(LIBS_DIR ../../libs)
set(PUGI ${LIBS_DIR}/xml)

include_directories(${PUGI})

set(SOURCE_FILES
        main.cpp
        ../../libs/xml/pugixml.cpp
        ../../libs/xml/pugiconfig.hpp
        ../../libs/xml/pugixml.hpp
        MapGenerator.cpp
        MapGenerator.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(Z_Generator ${SOURCE_FILES})
