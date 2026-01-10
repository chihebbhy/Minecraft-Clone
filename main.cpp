// ===== OpenGL loader =====
#include <glad/glad.h>

// ===== SDL3 =====
#include <SDL3/SDL.h>
#include <iostream>

/* ================= SHADERS ================= */
// Vertex shader: transforms vertex positions to clip space
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
})"
;

// Fragment shader: outputs color for each pixel
const char* fragmentShaderSrc = R"(
#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
)";

int main()
{
    // ===== SDL3 init =====
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Request OpenGL 3.3 Core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    SDL_Window* window = SDL_CreateWindow("SDL3 + OpenGL Square",
                                          800, 600,
                                          SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Window failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Context failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Load OpenGL functions via GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // ===== Define square vertices (two triangles) =====
 /*
 COMMENT beginning
 float vertices[] = {
    //    -->    Î
    //go right|| go up
        -0.5f, -0.5f,  // bottom-left
         0.5f, -0.5f,  // bottom-right
         0.5f,  0.5f,  // top-right
        -0.5f,  0.5f,  // top-left
         0.0f,  0.0f   // middle
    };

    unsigned int indices[] = {
        0, 1, 5,  // first triangle
        2, 3, 5   // second triangle
    };
COMMENT END
*/
    float vertices[] = {
    // x  |   y   | red  | green | blue
    -0.5f , -0.5f , 1.0f , 0.0f , 0.0f ,
     0.5f , -0.5f , 1.0f , 0.0f , 0.0f ,
     0.0f , 0.0f  , 0.0f , 1.0f , 0.0f ,

     // triangle 2

    -0.5f , 0.5f , 0.0f , 0.0f , 1.0f ,
     0.5f , 0.5f , 0.0f , 0.0f , 1.0f ,
     0.0f , 0.0f , 0.0f , 1.0f , 0.0f



    };
    // ===== Create VAO, VBO, EBO =====
    unsigned int VAO, VBO/*, EBO*/;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Index buffer
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Tell OpenGL how to interpret vertex data
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color (location = 1)
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(2 * sizeof(float))
);
glEnableVertexAttribArray(1);
    // ===== Compile shaders =====
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ===== Render loop =====
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw square
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,6);

        SDL_GL_SwapWindow(window);
    }

    // ===== Cleanup =====
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
