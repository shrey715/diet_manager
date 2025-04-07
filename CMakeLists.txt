cmake_minimum_required(VERSION 3.10)
project(diet_manager VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find nlohmann_json package
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# Add executable target
add_executable(diet_manager ${SOURCES})

# Link against nlohmann_json
target_link_libraries(diet_manager PRIVATE nlohmann_json::nlohmann_json)

# Create data directory in build folder
add_custom_command(
    TARGET diet_manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/data"
)

# Add install target
install(TARGETS diet_manager DESTINATION bin)
install(DIRECTORY data/ DESTINATION data)
