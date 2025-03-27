cmake_minimum_required(VERSION 3.10)
project(diet_manager)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR})

# Define source files
set(SOURCES
    src/main.cpp
    src/cli.cpp
    src/models/basic_food.cpp
    src/models/composite_food.cpp
    src/models/user.cpp
    src/models/log_entry.cpp
    src/managers/food_database.cpp
    src/managers/log_manager.cpp
)

# Create executable
add_executable(diet_manager ${SOURCES})

# Link libraries
target_link_libraries(diet_manager nlohmann_json::nlohmann_json)
target_link_libraries(diet_manager stdc++fs)
