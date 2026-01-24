#include "Game.hpp"

// Bibliotecas
#include <glad/glad.h>
#include <stb_image.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- SHADERS ---
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0";

// --- IMPLEMENTAÇÃO ---

Game::Game() {}
Game::~Game() {}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    m_window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (!m_window) return false;

    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context) return false;

    // VSYNC LIGADO (Evita tearing e limita uso de CPU)
    SDL_GL_SetSwapInterval(1); 

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return false;

    glEnable(GL_DEPTH_TEST);

    setupMesh();

    // WSL BUG FIX: Não travamos o mouse, usamos teclado para olhar.
    // SDL_SetRelativeMouseMode(SDL_TRUE); 

    m_isRunning = true;
    return true;
}

void Game::setupMesh() {
    // Compilação Shaders (Resumida)
    GLuint vS = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vS, 1, &vertexShaderSource, NULL); glCompileShader(vS);
    GLuint fS = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fS, 1, &fragmentShaderSource, NULL); glCompileShader(fS);
    m_shaderProgram = glCreateProgram(); glAttachShader(m_shaderProgram, vS); glAttachShader(m_shaderProgram, fS); glLinkProgram(m_shaderProgram);
    glDeleteShader(vS); glDeleteShader(fS);

    // Cubo
    float vertices[] = {
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, -0.5f, -0.5f,  0.5f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20
    };

    glGenVertexArrays(1, &m_VAO); glGenBuffers(1, &m_VBO); glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO); glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    glGenTextures(1, &m_texture); glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int w, h, nr; stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("wall.jpg", &w, &h, &nr, 0);
    if (data) {
        GLenum fmt = (nr == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}

void Game::run() {
    Uint64 perfFreq = SDL_GetPerformanceFrequency();
    Uint64 lastTime = SDL_GetPerformanceCounter();
    Uint64 currentTime;

    while (m_isRunning) {
        currentTime = SDL_GetPerformanceCounter();
        float deltaTime = (float)(currentTime - lastTime) / (float)perfFreq;
        lastTime = currentTime;
        if (deltaTime > 0.05f) deltaTime = 0.05f; // Cap de lag

        processInput(deltaTime);
        update(deltaTime);
        render();
    }
    SDL_GL_DeleteContext(m_context); SDL_DestroyWindow(m_window); SDL_Quit();
}

void Game::processInput(float deltaTime) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) m_isRunning = false;
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    // Velocidade Base: 2.5 | Sprint (Shift): 5.0
    float speed = (state[SDL_SCANCODE_LSHIFT]) ? 5.0f : 2.5f;
    float velocity = speed * deltaTime;
    
    // Sensibilidade das setas
    float sensitivity = 80.0f * deltaTime;

    // --- MOVIMENTO (WASD) ---
    if (state[SDL_SCANCODE_W]) cameraPos += velocity * cameraFront;
    if (state[SDL_SCANCODE_S]) cameraPos -= velocity * cameraFront;
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    if (state[SDL_SCANCODE_A]) cameraPos -= right * velocity;
    if (state[SDL_SCANCODE_D]) cameraPos += right * velocity;

    // --- OLHAR (SETAS - Compatível com WSL) ---
    if (state[SDL_SCANCODE_RIGHT]) yaw += sensitivity;
    if (state[SDL_SCANCODE_LEFT])  yaw -= sensitivity;
    if (state[SDL_SCANCODE_UP])    pitch += sensitivity;
    if (state[SDL_SCANCODE_DOWN])  pitch -= sensitivity;

    // Trava de Pescoço
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Atualiza Vetores
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    if (state[SDL_SCANCODE_ESCAPE]) m_isRunning = false;
}

void Game::update(float deltaTime) {}

void Game::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glUseProgram(m_shaderProgram);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 model = glm::mat4(1.0f); 
    // Deixei o cubo parado para você navegar ao redor dele
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(m_window);
}