# Figure out how to package libraries together in geodesy.
cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0077 NEW)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME "geodesy-unit-test")

# ----------------------- Dependencies ----------------------- #

include(FetchContent)

# Utility function for fetching dependencies in one line.
function(fetch_dependency ADEP_NAME AGIT_REPO AGIT_TAG)
    # Remaining args are variable=value pairs for cache options
    set(options)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(FETCHDEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Declare the dependency
    FetchContent_Declare(
        ${ADEP_NAME}
        GIT_REPOSITORY ${AGIT_REPO}
        GIT_TAG        ${AGIT_TAG}
        SOURCE_DIR     ${CMAKE_SOURCE_DIR}/dep/${ADEP_NAME}
    )
    
    # Apply cache variables with intelligent type detection and FORCE override
    # CRITICAL: Set these BEFORE FetchContent_Declare
    foreach(arg IN LISTS ARGN)
        string(REPLACE "=" ";" kv "${arg}")
        list(GET kv 0 key)
        list(GET kv 1 val)
       
        # Determine the appropriate cache variable type based on value
        if(val MATCHES "^(ON|OFF|TRUE|FALSE)$")
            # Boolean values
            set(${key} ${val} CACHE BOOL "Forced by fetch_dependency for ${ADEP_NAME}" FORCE)
        else()
            # Everything else as STRING
            set(${key} ${val} CACHE STRING "Forced by fetch_dependency for ${ADEP_NAME}" FORCE)
        endif()
    endforeach()
    
    # Actually fetch it - cache variables should now be available to subdirectory
    FetchContent_MakeAvailable(${ADEP_NAME})
endfunction()

# Include Geodesy Engine dependency
fetch_dependency(geodesy-engine https://github.com/FermionicChaos/geodesy-engine.git master VULKAN_SDK_STATIC_LIB=ON VULKAN_SDK_VERSION=1.4.321.0)

# ----------------------- Geodesy Unit Test ----------------------- #
# When creating your own project off of the Geodesy Library, use this
# as a example/template for your own CMakeLists.txt file to know what to link against.

file(GLOB_RECURSE INC
    "inc/*.h"
)

file(GLOB_RECURSE SRC
    "src/*.h"
    "src/*.cpp"
    "src/*.c"
)

# Declare project
project(${PROJECT_NAME})

# Main Game Executable (Your Code)
add_executable(${PROJECT_NAME} ${INC} ${SRC} main.cpp res/icon.rc)

# Output Directory for Binaries.
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc/)

# Link Against Geodesy Library
target_link_libraries(${PROJECT_NAME} PRIVATE geodesy-engine)
