# CMakeList.txt : CMake project for Human Blastocyst, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (Pottchi)

# Add source to this project's executable.

IF(APPLE)
include_directories("/usr/local/include")
ENDIF()


file(GLOB SRC 
    "src/Main.cpp"
    "src/RandomNumberGenerators.cpp"
    "src/SquareCellGrid.cpp"
    "src/SuperCell.cpp"
    "src/CellType.cpp"
    "src/ColourScheme.cpp"
    "src/TransformEvent.cpp"
    "src/ReportEvent.cpp"
    "src/SuperCellTemplate.cpp"
    "src/DivisionHandler.cpp"
    "src/TransformHandler.cpp"
    "src/ReportHandler.cpp"
)

file(GLOB HDR
    "src/headers/ColourScheme.h"
    "src/headers/RandomNumberGenerators.h"
    "src/headers/SquareCellGrid.h"
    "src/headers/SuperCell.h"
    "src/headers/Vector2D.h"
    "src/headers/MathConstants.h"
    "src/headers/split.h"
    "src/headers/CellType.h"
    "src/headers/ReportEvent.h"
    "src/headers/TransformEvent.h"
    "src/headers/SuperCellTemplate.h"
    "src/headers/DivisionHandler.h"
    "src/headers/TransformHandler.h"
    "src/headers/ReportHandler.h"
)

file(GLOB LIB "lib/cxxopts.hpp")

add_executable (Pottchi ${SRC} ${HDR} ${LIB})

set_property(TARGET Pottchi PROPERTY CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

option(SSH_HEADLESS "Remove all SFML for running remotely" OFF)
if (SSH_HEADLESS)
  add_definitions(-DSSH_HEADLESS)
endif()

set(SFML_FIND_QUIETLY FALSE)
find_package(SFML COMPONENTS graphics window system REQUIRED)

target_link_libraries(Pottchi PRIVATE sfml-graphics sfml-window ${SFML_DEPENDENCIES})

IF(NOT WIN32)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")
ENDIF()

#Copy default settings

add_custom_target(copy-cfg ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default.cfg")
add_custom_command(COMMAND ${CMAKE_COMMAND} -E copy 
                   ${CMAKE_CURRENT_SOURCE_DIR}/default.cfg
                   ${CMAKE_CURRENT_BINARY_DIR}/default.cfg
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/default.cfg
                   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/default.cfg
                   COMMENT "Copied default config"
)

add_custom_target(copy-img ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/default.pgm")
add_custom_command(COMMAND ${CMAKE_COMMAND} -E copy 
                   ${CMAKE_CURRENT_SOURCE_DIR}/default.pgm
                   ${CMAKE_CURRENT_BINARY_DIR}/default.pgm
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/default.pgm
                   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/default.pgm
                   COMMENT "Copied default layout"
)
add_dependencies(Pottchi copy-cfg copy-img)
