# Figure out how to package libraries together in geodesy.
cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0077 NEW)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GEODESY_UNIT_TEST "geodesy-unit-test")

# ----------------------- Dependencies ----------------------- #

include(FetchContent)

# Setup directories for dependencies
#set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/dep")

# ----- Assimp ----- #
FetchContent_Declare(
    geodesy
    GIT_REPOSITORY      https://github.com/FermionicChaos/geodesy.git
    GIT_TAG             renderer
)
FetchContent_MakeAvailable(geodesy)

# ----------------------- Geodesy Unit Test ----------------------- #
# When creating your own project off of the Geodesy Library, use this
# as a template for your own CMakeLists.txt file to know what to link against.

project(${GEODESY_UNIT_TEST})

# Link against Vulkan and OpenMP
add_executable(${GEODESY_UNIT_TEST} main.cpp)

set_target_properties(${GEODESY_UNIT_TEST} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/)

# Include directories
target_include_directories(${GEODESY_UNIT_TEST} PRIVATE ${Vulkan_INCLUDE_DIRS}/)
target_include_directories(${GEODESY_UNIT_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/inc/)
target_include_directories(${GEODESY_UNIT_TEST} PRIVATE ${FETCHCONTENT_BASE_DIR}/inc/)

# Link Against Geodesy Library
target_link_libraries(${GEODESY_UNIT_TEST} PRIVATE geodesy)
