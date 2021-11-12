# CMakeList.txt : CMake project for Human Blastocyst, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (HumanBlastocyst)

# Add source to this project's executable.

file(GLOB SRC "src/Main.cpp" "src/Cell.cpp" "src/RandomNumberGenerators.cpp" "src/SquareCellGrid.cpp" "src/SuperCell.cpp")
file(GLOB HDR "src/headers/Cell.h" "src/headers/CellTypes.h" "src/headers/RandomNumberGenerators.h" "src/headers/SquareCellGrid.h" "src/headers/SuperCell.h" "src/headers/Vector2D.h")

add_executable (HumanBlastocyst ${SRC} ${HDR})

# TODO: Add tests and install targets if needed.

find_package(SDL2 CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(HumanBlastocyst PRIVATE SDL2::SDL2 SDL2::SDL2main ${Boost_LIBRARIES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
