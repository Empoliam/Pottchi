# CMakeList.txt : CMake project for Human Blastocyst, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (Pottchi)

# Add source to this project's executable.

file(GLOB SRC "src/Main.cpp" "src/RandomNumberGenerators.cpp" "src/SquareCellGrid.cpp" "src/SuperCell.cpp")
file(GLOB HDR "src/headers/RandomNumberGenerators.h" "src/headers/SquareCellGrid.h" "src/headers/SuperCell.h" "src/headers/Vector2D.h" "src/headers/MathConstants.h" "src/headers/split.h" "src/headers/CellType.h")

add_executable (Pottchi ${SRC} ${HDR} "src/CellType.cpp" "src/headers/ColourScheme.h" "src/ColourScheme.cpp" "src/headers/TransformEvent.h" "src/TransformEvent.cpp")

set_property(TARGET Pottchi PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.

find_package(SDL2 CONFIG REQUIRED)


target_link_libraries(Pottchi PRIVATE SDL2::SDL2 SDL2::SDL2main)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")

#Copy default settings
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/default.cfg ${CMAKE_CURRENT_BINARY_DIR}/default.cfg COPYONLY)

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
