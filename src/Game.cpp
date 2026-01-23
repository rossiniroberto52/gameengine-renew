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
    // A. Compilação dos Shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // (Dica: Checar erros de compilação aqui seria ideal)

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // B. Dados do Quadrado (Vértices e UVs)
    float vertices[] = {
        // posições (X,Y,Z)    // textura (U,V)
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f, // Topo Direito (0)
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f, // Baixo Direito (1)
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // Baixo Esquerdo (2)
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f  // Topo Esquerdo (3)
    };

    // C. Índices para o EBO (Dois Triângulos formam o Quadrado)
    unsigned int indices[] = {  
        0, 1, 3,  // Primeiro Triângulo
        1, 2, 3   // Segundo Triângulo
    };

    // D. Geração dos Buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // VBO: Envia os vértices
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO: Envia os índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // E. Configuração dos Atributos (Layout)
    // Atributo 0: Posição (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: Textura (2 floats) - Offset de 3 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // F. Carregamento da Textura
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Configura Repetição e Filtro
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a imagem do disco
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Corrige o Y invertido do OpenGL
    unsigned char *data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
    
    if (data) {
        // Detecta se é JPG (RGB) ou PNG (RGBA)
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "FALHA CRÍTICA: Não foi possível carregar wall.jpg" << std::endl;
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
}

void Game::update(float deltaTime) {
    // Lógica do jogo (movimento, física) entraria aqui
}

void Game::render() {
    // 1. Limpa Cor e Profundidade (Z-Buffer)
    // Se não limpar o DEPTH, o 3D buga e um lado atravessa o outro.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shaderProgram);

    // --- 1. PROJECTION (A Lente da Câmera) ---
    // Resolve o problema do achatamento!
    // FOV: 45 graus
    // Aspect Ratio: 800 / 600 (Dimensões da sua janela)
    // Near/Far: Só desenha o que estiver entre 0.1 e 100.0 unidades de distância
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    
    unsigned int projLoc = glGetUniformLocation(m_shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // --- 2. VIEW (A Posição da Câmera) ---
    // Movemos o MUNDO 3 unidades para trás (-3 no Z), o que dá a ilusão
    // de que a câmera foi 3 unidades para trás.
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    
    unsigned int viewLoc = glGetUniformLocation(m_shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // --- 3. MODEL (O Objeto) ---
    // Gira o quadrado no eixo (0.5, 1.0, 0.0) para ficar mais estiloso
    glm::mat4 model = glm::mat4(1.0f);
    float time = SDL_GetTicks() / 1000.0f;
    model = glm::rotate(model, time, glm::vec3(0.5f, 1.0f, 0.0f));

    // ATENÇÃO: O nome no shader mudou de "transform" para "model"
    unsigned int modelLoc = glGetUniformLocation(m_shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // --- 4. DESENHAR ---
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(m_window);
}