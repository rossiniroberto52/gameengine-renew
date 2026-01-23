cmake_minimum_required(VERSION 3.15)
project(Engine2D_REMAKE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CORREÇÃO AQUI: 'third-party' com hífen
add_library(glad STATIC third-party/glad/src/glad.c)
target_include_directories(glad PUBLIC third-party/glad/include)

# CORREÇÃO AQUI: 'third-party' com hífen
add_subdirectory(third-party/SDL2-2.32.10)

add_executable(Engine2D_REMAKE 
    src/main.cpp 
    src/Game.cpp 
)

target_link_libraries(Engine2D_REMAKE PRIVATE glad SDL2::SDL2main SDL2::SDL2-static)