#include "Game.hpp"

// Bibliotecas Gráficas
#include <glad/glad.h>
#include <stb_image.h> // Carregamento de imagens
#include <cmath>

// GLM (Matemática)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- SHADERS (GLSL) ---

// Vertex Shader: Recebe Posição (loc 0) e Textura (loc 1)
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    // REMOVI O COMENTÁRIO PRA EVITAR O BUG
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = aTexCoord;\n"
    "}\0"; // Corrigido o erro de aspas "}"; no final
// Fragment Shader: Usa a textura para pintar o pixel
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n" // A Imagem
    "void main()\n"
    "{\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0";

// --- IMPLEMENTAÇÃO DA CLASSE GAME ---

Game::Game() {}
Game::~Game() {}

// Inicialização de tudo (Janela, GLAD, OpenGL, Mesh)
bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    // 1. INICIALIZA SDL (VOCÊ TINHA APAGADO ISSO)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    // 2. Configura OpenGL (Versão 3.3 Core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    // 3. Cria a Janela
    m_window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (!m_window) {
        std::cerr << "Erro Janela: " << SDL_GetError() << std::endl;
        return false;
    }

    // 4. Cria Contexto OpenGL
    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context) {
        std::cerr << "Erro Contexto: " << SDL_GetError() << std::endl;
        return false;
    }

    // 5. Carrega Ponteiros do OpenGL (GLAD)
    // SÓ AGORA PODEMOS CARREGAR O GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Erro GLAD" << std::endl;
        return false;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // 6. Configurações Globais do OpenGL
    glEnable(GL_DEPTH_TEST); // <--- O DEPTH TEST FICA AQUI, APÓS O GLAD!

    // 7. Configura a Mesh
    setupMesh();

    m_isRunning = true;
    return true;
}

void Game::setupMesh() {
    // 1. SHADERS (MANTENHA IGUAL - Copie e cole se precisar, mas é o mesmo)
    // ... (Compilação dos shaders igualzinha ao anterior) ...
    // Vou pular a compilação aqui para economizar espaço, mas NÃO APAGUE ELA!
    // Se você apagou, copie do código anterior.
    // --------------------------------------------------------------------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // --------------------------------------------------------------------

    // 2. VÉRTICES (24 Vértices: 6 Faces x 4 Pontos)
    // Formato: X, Y, Z, U, V
    float vertices[] = {
        // Frente
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 0
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 1
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 2
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 3
        // Trás
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 4
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 5
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 6
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 7
        // Esquerda
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 8
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 9
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 10
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 11
        // Direita
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 12
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 13
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 14
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 15
        // Topo
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 16
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 17
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 18
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // 19
        // Base
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 20
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 21
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // 22
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f  // 23
    };

    // 3. ÍNDICES (36 Índices: 6 Faces x 2 Triângulos x 3 Pontos)
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,       // Frente
        4, 5, 6, 6, 7, 4,       // Trás
        8, 9, 10, 10, 11, 8,    // Esquerda
        12, 13, 14, 14, 15, 12, // Direita
        16, 17, 18, 18, 19, 16, // Topo
        20, 21, 22, 22, 23, 20  // Base
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 4. TEXTURA (MANTENHA IGUAL)
    // ... (Copie o carregamento da textura wall.jpg aqui) ...
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}

void Game::run() {
    Uint64 lastTime = SDL_GetTicks64();
    Uint64 currentTime;

    while (m_isRunning) {
        // Cálculo do DeltaTime
        currentTime = SDL_GetTicks64();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        processInput();
        update(deltaTime);
        render();
    }
    
    // Limpeza
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Game::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
    }

    // --- LEITURA DO TECLADO (ESTADO CONTÍNUO) ---
    const Uint8* state = SDL_GetKeyboardState(NULL);

    // Velocidade da câmera (ajustada pelo "feeling", idealmente usaria deltaTime aqui)
    float cameraSpeed = 0.05f; 

    // W = Frente
    if (state[SDL_SCANCODE_W]) {
        cameraPos += cameraSpeed * cameraFront;
    }
    // S = Trás
    if (state[SDL_SCANCODE_S]) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    // A = Esquerda (Normalizamos o produto vetorial para não andar mais rápido na diagonal)
    if (state[SDL_SCANCODE_A]) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    // D = Direita
    if (state[SDL_SCANCODE_D]) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    
    // ESC para sair rápido
    if (state[SDL_SCANCODE_ESCAPE]) {
        m_isRunning = false;
    }
}

void Game::update(float deltaTime) {
    // Lógica do jogo (movimento, física) entraria aqui
}

void Game::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Limpa COR e PROFUNDIDADE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glUseProgram(m_shaderProgram);

    // Projection
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // View (Câmera)
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Model (Cubo Girando)
    glm::mat4 model = glm::mat4(1.0f);
    float time = SDL_GetTicks() / 1000.0f;
    // Gira em X e Y para mostrar todas as faces
    model = glm::rotate(model, time, glm::vec3(0.5f, 1.0f, 0.0f)); 
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glBindVertexArray(m_VAO);
    
    // --- MUDANÇA CRÍTICA AQUI ---
    // Agora desenhamos 36 vértices (6 faces * 2 triângulos * 3 pontas)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // ----------------------------

    SDL_GL_SwapWindow(m_window);
}