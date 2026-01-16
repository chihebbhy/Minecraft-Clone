#include "Block.h"
#include <iostream>
#include "stb_image.h"

Block::Block(float x, float y, float z)
{
    posX = x;
    posY = y;
    posZ = z;

    // ===== Cube vertices (pos + uv) =====
    float vertices[] = {
        // Front
        -0.5f, -0.5f,  0.5f, 0, 0,
         0.5f, -0.5f,  0.5f, 1, 0,
         0.5f,  0.5f,  0.5f, 1, 1,
         0.5f,  0.5f,  0.5f, 1, 1,
        -0.5f,  0.5f,  0.5f, 0, 1,
        -0.5f, -0.5f,  0.5f, 0, 0,

        // Back
        -0.5f, -0.5f, -0.5f, 1, 0,
        -0.5f,  0.5f, -0.5f, 1, 1,
         0.5f,  0.5f, -0.5f, 0, 1,
         0.5f,  0.5f, -0.5f, 0, 1,
         0.5f, -0.5f, -0.5f, 0, 0,
        -0.5f, -0.5f, -0.5f, 1, 0,

        // Left
        -0.5f,  0.5f,  0.5f, 1, 1,
        -0.5f,  0.5f, -0.5f, 0, 1,
        -0.5f, -0.5f, -0.5f, 0, 0,
        -0.5f, -0.5f, -0.5f, 0, 0,
        -0.5f, -0.5f,  0.5f, 1, 0,
        -0.5f,  0.5f,  0.5f, 1, 1,

        // Right
         0.5f,  0.5f,  0.5f, 0, 1,
         0.5f, -0.5f,  0.5f, 0, 0,
         0.5f, -0.5f, -0.5f, 1, 0,
         0.5f, -0.5f, -0.5f, 1, 0,
         0.5f,  0.5f, -0.5f, 1, 1,
         0.5f,  0.5f,  0.5f, 0, 1,

        // Top
        -0.5f,  0.5f, -0.5f, 0, 1,
        -0.5f,  0.5f,  0.5f, 0, 0,
         0.5f,  0.5f,  0.5f, 1, 0,
         0.5f,  0.5f,  0.5f, 1, 0,
         0.5f,  0.5f, -0.5f, 1, 1,
        -0.5f,  0.5f, -0.5f, 0, 1,

        // Bottom
        -0.5f, -0.5f, -0.5f, 1, 1,
         0.5f, -0.5f, -0.5f, 0, 1,
         0.5f, -0.5f,  0.5f, 0, 0,
         0.5f, -0.5f,  0.5f, 0, 0,
        -0.5f, -0.5f,  0.5f, 1, 0,
        -0.5f, -0.5f, -0.5f, 1, 1
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ===== Load 6 textures =====
    LoadTexture("Textures/Blocks/GrassBlock/grass_block_side.png", textures[0]);
    LoadTexture("Textures/Blocks/GrassBlock/grass_block_side.png", textures[1]);
    LoadTexture("Textures/Blocks/GrassBlock/grass_block_side.png", textures[2]);
    LoadTexture("Textures/Blocks/GrassBlock/grass_block_side.png", textures[3]);
    LoadTexture("Textures/Blocks/GrassBlock/OldGrassTop.png", textures[4]);
    LoadTexture("Textures/Blocks/GrassBlock/OldGrassTop.png", textures[5]);
}

void Block::LoadTexture(const char* path, unsigned int& textureID)
{
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 4);

    if (!data)
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void Block::Draw()
{
    glBindVertexArray(VAO);

    for (int i = 0; i < 6; i++)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
}

Block::~Block()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(6, textures);
}
