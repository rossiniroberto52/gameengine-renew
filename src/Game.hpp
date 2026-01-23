#pragma once

// GLAD deve vir ANTES de qualquer include que tenha OpenGL (como SDL)
#include <glad/glad.h> 

#include <SDL.h>
#include <iostream>
#include <glm/glm.hpp> // GLM para matemática vetorial

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void run();

private:
    // Janela e Contexto
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    bool m_isRunning = false;

    // Funções Internas
    // ATENÇÃO: Agora processInput recebe deltaTime
    void processInput(float deltaTime); 
    void update(float deltaTime);
    void render();

    // Setup Gráfico
    void setupMesh();

    // IDs do OpenGL
    GLuint m_shaderProgram = 0;
    GLuint m_VAO = 0; // Vertex Array
    GLuint m_VBO = 0; // Vertex Buffer
    GLuint m_EBO = 0; // Element Buffer
    GLuint m_texture = 0; // Textura

    // --- CÂMERA (VETORES) ---
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    // --- CÂMERA (ROTAÇÃO / MOUSE) ---
    float yaw   = -90.0f; // Rotação Horizontal
    float pitch =   0.0f; // Rotação Vertical
};