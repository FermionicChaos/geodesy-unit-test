# Figure out how to package libraries together in geodesy.
cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0077 NEW)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------- Dependencies ----------------------- #

include(FetchContent)

# Setup source directories for dependencies (build dirs will remain in build folder)
set(DEP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/dep")

# ----- Core Geodesy Engine ----- #
FetchContent_Declare(
    geodesy
    GIT_REPOSITORY      https://github.com/FermionicChaos/geodesy.git
    GIT_TAG             vr-development
    SOURCE_DIR          ${DEP_SOURCE_DIR}/geodesy-src
)
FetchContent_MakeAvailable(geodesy)

# ----------------------- Geodesy Unit Test ----------------------- #
# When creating your own project off of the Geodesy Library, use this
# as a example/template for your own CMakeLists.txt file to know what to link against.

set(GEODESY_UNIT_TEST "geodesy-unit-test")

file(GLOB_RECURSE INC
    "inc/*.h"
)

file(GLOB_RECURSE SRC
    "src/*.h"
    "src/*.cpp"
    "src/*.c"
)

# Declare project
project(${GEODESY_UNIT_TEST})

# Main Game Executable (Your Code)
add_executable(${GEODESY_UNIT_TEST} ${INC} ${SRC} main.cpp res/icon.rc)

# Output Directory for Binaries.
set_target_properties(${GEODESY_UNIT_TEST} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/)

# Include directories
target_include_directories(${GEODESY_UNIT_TEST} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

# Link Against Geodesy Library
target_link_libraries(${GEODESY_UNIT_TEST} PRIVATE geodesy)
