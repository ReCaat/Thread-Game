cmake_minimum_required(VERSION 3.10)
project(Thread-Game CXX)

# Biblioteca do raylib
find_package(raylib CONFIG REQUIRED)

include_directories(src)
set(SOURCES
    src/main.cpp
    src/cobra.cpp
    src/comida.cpp
)

# Executável
add_executable(exec ${SOURCES})
target_link_libraries(exec PRIVATE raylib)