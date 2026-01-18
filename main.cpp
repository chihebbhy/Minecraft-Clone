// ===== OpenGL loader =====
#include <glad/glad.h>

// ===== SDL3 =====
#include <SDL3/SDL.h>
#include <iostream>

// ===== HEADERS =====
#include "Camera.h"
#include "input.h"
#include "block.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
/* ================= SHADERS ================= */
// Vertex shader: transforms vertex positions to clip space
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    TexCoord = aTex;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
})"
                              ;

// Fragment shader: outputs color for each pixel
const char* fragmentShaderSrc = R"(
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    FragColor = texture(tex,TexCoord);
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
    SDL_Window* window = SDL_CreateWindow("MINECRAFT!!!!!!!!!!!!!",
                                          800, 600,
                                          SDL_WINDOW_OPENGL);
    SDL_SetWindowFullscreen(window,true);
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

    glUseProgram(vertexShader);
    glUniform1i(glGetUniformLocation(shaderProgram,"tex"),0);

    glEnable(GL_DEPTH_TEST);
    SDL_SetWindowRelativeMouseMode(window, true);

    int w,h;
    SDL_GetWindowSize(window,&w,&h);
    Camera Cam((float) w, (float) h);
    Block* tab[100];

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            tab[i*10 + j] = new Block((float)i, 0.0f, (float)j);
    }
}

    // ===== Render loop =====
    bool running = true;
    while (running)
    {
        SDL_Event e;
        const bool *keyboardState = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&e))
        {
            if(!Input::HandleEvent(e,Cam)){
                running = false;
            }
        }
        // Clear screen
        glClearColor(0.46f, 0.65f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Cam.upload(shaderProgram);
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                tab[i*10 + j]->Draw(shaderProgram);
    }
}
        SDL_GL_SwapWindow(window);
    }
    for (int i = 0; i < 100; i++)
        delete tab[i];

    glDeleteProgram(shaderProgram);

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
