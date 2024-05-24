# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(luacurses VERSION 1.0 LANGUAGES C)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Find Lua
find_package(Lua 5.4 REQUIRED)

# Find ncurses
find_package(Curses REQUIRED)

# Include directories
include_directories(${LUA_INCLUDE_DIR} ${CURSES_INCLUDE_DIR})

# Source files
set(SOURCES src/luacurses.c)

# Add the executable (replace "luacurses" with the desired target name)
add_library(luacurses SHARED ${SOURCES})

# Link libraries
target_link_libraries(luacurses ${LUA_LIBRARIES} ${CURSES_LIBRARIES})

# Set the output directory for the compiled library
set_target_properties(luacurses PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install rules
install(TARGETS luacurses
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)
install(FILES luacurses.h DESTINATION include)
