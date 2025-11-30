cmake_minimum_required(VERSION 3.10)
project(Thread-Game CXX)

# Biblioteca do raylib
add_subdirectory(libs/raylib)
# find_package(raylib CONFIG REQUIRED)

# Source code
include_directories(src)
set(SOURCES
    src/main.cpp
    src/cobra.cpp
    src/comida.cpp
)

# Executável
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Troço esquisito do mac pra kalinda
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()