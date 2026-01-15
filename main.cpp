// ===== OpenGL loader =====
#include <glad/glad.h>

// ===== SDL3 =====
#include <SDL3/SDL.h>
#include <iostream>

// ===== HEADERS =====
#include "Camera.h"

/* ================= SHADERS ================= */
// Vertex shader: transforms vertex positions to clip space
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0); // w = 1 fixed
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
    SDL_Window* window = SDL_CreateWindow("MINECRAFT!!!!!!!!!!!!!",
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
    float vertices[] = {
    // x  |   y   |   z  | red  | green | blue
    -0.5f , -0.5f , -0.5f , 0.0f , 1.0f , 0.0f ,
     0.5f , -0.5f , -0.5f , 1.0f , 0.0f , 0.0f ,
     0.0f , 0.0f  , -0.5f , 1.0f , 0.0f , 0.0f ,

     // triangle 2

    -0.5f , 0.5f , -2.5f , 0.0f , 0.0f , 1.0f ,
     0.5f , 0.5f , -2.5f , 0.0f , 0.0f , 1.0f ,
     0.0f , -1.0f , -2.5f , 0.0f , 0.0f , 1.0f



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

    // Tell OpenGL how to interpret vertex data
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    // color (location = 1)
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
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

    glEnable(GL_DEPTH_TEST);
    SDL_SetWindowRelativeMouseMode(window, true);

    Camera Cam(800.0f,600.0f);
    Cam.upload(shaderProgram);
    // ===== Render loop =====
    bool running = true;
    while (running)
    {
        SDL_Event e;
        const bool *keyboardState = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&e))
        {
            switch(e.type){
            case SDL_EVENT_QUIT :
                    running = false;
                    break;
            case SDL_EVENT_KEY_DOWN:
                switch(e.key.key){
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_UP:
                case SDLK_Z:
                    Cam.MoveForward(0.1f);
                    break;
                case SDLK_DOWN:
                case SDLK_S:
                    Cam.MoveForward(-0.1f);
                    break;
                case SDLK_RIGHT:
                case SDLK_D:
                    Cam.MoveRight(0.1f);
                    break;
                case SDLK_LEFT:
                case SDLK_Q:
                    Cam.MoveRight(-0.1f);
                    break;
                case SDLK_SPACE:
                    Cam.MoveUp(0.1f);
                    break;
                case SDLK_LSHIFT:
                    Cam.MoveUp(-0.1f);
                    break;
                case SDLK_H:
                    std::cout << "Yaw: " << Cam.GetYaw() << " Pitch: " << Cam.GetPitch() << std::endl;
                    break;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                Cam.ProcessMouse(
                    (float)e.motion.xrel,
                    (float)e.motion.yrel
    );
    break;

        }
        }
        // Clear screen
        glClearColor(0.46f, 0.65f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw square
        glUseProgram(shaderProgram);

        Cam.upload(shaderProgram);
        // Simple perspective matrix
        float aspect = 800.0f / 600.0f;
        float fov = 1.0f;
        float near = 0.1f;
        float far  = 10.0f;

        float projection[16] = {
            1.0f/(aspect*fov), 0, 0, 0,
            0, 1.0f/fov, 0, 0,
            0, 0, -(far+near)/(far-near), -1,
            0, 0, -(2*far*near)/(far-near), 0
        };

        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

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
