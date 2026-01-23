#pragma once // Garante que o arquivo só seja incluído uma vez
#include <glad/glad.h> // opengl build
#include <SDL.h> // Necessário para saber o que é SDL_Window*
#include <glm/glm.hpp> // Biblioteca de Matemática
#include <iostream>


class Game {
public:
    Game();  // Construtor
    ~Game(); // Destrutor (Limpa a memória)

    // A função de inicialização não retorna a janela, retorna se DEU CERTO (bool)
    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    
    // Onde o jogo roda (Update, Render)
    void run();

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    bool m_isRunning = false;

    // Funções internas
    void processInput();
    void update(float deltaTime); // Olha o DeltaTime aqui!
    void render();

    // Setup's
    void setupMesh();
    
    GLuint m_shaderProgram = 0;
    GLuint m_VAO = 0; // Vertex Array Object
    GLuint m_VBO = 0; // Vertex Buffer Object
    GLuint m_EBO = 0; // Element Buffer Object
    GLuint m_texture = 0; // Texture

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f); // Começa afastado (Z=3)
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Olha para frente (Z negativo)
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f); // Onde fica o "céu"

};