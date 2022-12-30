cmake_minimum_required(VERSION 3.24)
project(nbody_physics)
set(CMAKE_CXX_STANDARD 20)

# SFML Dependency Pull
include(FetchContent)
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# Build stuff
include_directories(nbody_physics src/nbody)
set(SOURCES src/nbody/bodies.cpp src/main.cpp)
add_executable(nbody_physics ${SOURCES})

# Link SFML Dependency
target_link_libraries(nbody_physics PRIVATE sfml-graphics)

# Set Compiler up
target_compile_features(nbody_physics PRIVATE cxx_std_20)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(
            TARGET nbody_physics POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:nbody_physics> $<TARGET_FILE_DIR:nbody_physics> COMMAND_EXPAND_LISTS
    )
endif()
