cmake_minimum_required(VERSION 3.15)
project(Engine2D_REMAKE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. CONFIGURAÇÃO DO GLAD ---
add_library(glad STATIC third-party/glad/src/glad.c)
target_include_directories(glad PUBLIC third-party/glad/include)

# --- 2. CONFIGURAÇÃO DA SDL2 ---
# (Certifique-se que o nome da pasta aqui bate com a sua pasta real)
add_subdirectory(third-party/SDL2-2.32.10)

# --- 3. SEU EXECUTÁVEL ---
add_executable(Engine2D_REMAKE 
    src/main.cpp 
    src/Game.cpp 
    src/stb_impl.cpp
)

# --- 4. INCLUDES DO PROJETO (AQUI ENTRA A GLM) ---
# Isso diz ao compilador onde procurar arquivos .h e .hpp
target_include_directories(Engine2D_REMAKE PRIVATE 
    src                      # Para achar seus proprios headers
    third-party/glm          # Para achar a GLM
    third-party/stb_image    # Para achar o stb
)

# --- 5. LINKAGEM DAS BIBLIOTECAS ---
target_link_libraries(Engine2D_REMAKE PRIVATE glad SDL2::SDL2main SDL2::SDL2-static)