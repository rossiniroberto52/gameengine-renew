#include "Game.hpp"
#include <glad/glad.h> // Glad no .cpp
#include <cmath>

// --- SHADERS (CÓDIGO GLSL) ---
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Fragment Shader: Define a cor final (Laranja aqui)
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n" // Variável que vem do C++
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n" // <--- USAR A VARIÁVEL AQUI
    "}\n\0";

// --- IMPLEMENTAÇÃO ---

void Game::setupTriangle() {
    // 1. Compilar Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // (Deveríamos checar erros de compilação aqui, mas vou pular pra testar sua sorte)

    // 2. Compilar Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // 3. Linkar Shaders no Programa Final
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    // Limpar os shaders individuais (já estão no programa)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 4. Definir os pontos do Triângulo (X, Y, Z)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Esquerda Embaixo
         0.5f, -0.5f, 0.0f, // Direita Embaixo
         0.0f,  0.5f, 0.0f  // Topo
    };

    // 5. Configurar Buffers (VBO e VAO)
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // Bind do VAO primeiro! Ele vai "gravar" as configurações do VBO.
    glBindVertexArray(m_VAO);

    // Copiar vértices para a memória da GPU
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Explicar para o OpenGL como ler esses dados (Atributo 0 = Posição, 3 floats, etc)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desbindar para não quebrar nada acidentalmente (boa prática)
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
}

Game::Game() {}

Game::~Game() {}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    // 1. Inicializa SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erro SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    // 2. Configura OpenGL (Atributos)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    // 3. Cria a Janela (Atribuindo à variável da classe m_window)
    m_window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (!m_window) {
        std::cerr << "Erro ao criar Janela: " << SDL_GetError() << std::endl;
        return false;
    }

    // 4. Cria o Contexto
    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context) {
        std::cerr << "Erro ao criar Contexto: " << SDL_GetError() << std::endl;
        return false;
    }

    // 5. Carrega GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Erro ao carregar GLAD" << std::endl;
        return false;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    m_isRunning = true;

    setupTriangle();

    return true;
}

void Game::run() {

    //logica temp = deltaT=Tatual-Tanterior, ent deltatime = (tempAtual - tempAnterior)/1000.0f
    Uint64 lastTime = SDL_GetTicks64();
    Uint64 currentTime;


    while (m_isRunning) {

        currentTime = SDL_GetTicks64();
        float deltatime = (currentTime-lastTime)/1000.0f;

        lastTime = currentTime;

        processInput();
        update(deltatime);
        render();
    }
    
    // Limpeza quando o loop acabar
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
}

void Game::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // --- DESENHAR ---
    glUseProgram(m_shaderProgram); // 1. Use o Shader
    float  timeValue = SDL_GetTicks() / 1000.0f;
    float greenValue = (std::sin(timeValue)/2.0f)+0.5f;

    int vertexColorLocation = glGetUniformLocation(m_shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glBindVertexArray(m_VAO); // 2. Bind do VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(m_window);
}

void Game::update(float deltaTime) {
    // Por enquanto, só imprime o DeltaTime para vermos se a física está rodando
    //std::cout << "DT: " << deltaTime << std::endl; 
    
    // Futuramente, aqui entrará:
    // player.update(deltaTime);
    // enemies.update(deltaTime);
}