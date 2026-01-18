#pragma once
#include <glad/glad.h>

class Block {

    float posX,posY,posZ;
    unsigned int VAO, VBO;

    unsigned int textures[6];

    void LoadTexture(const char* path, unsigned int& textureID);

public:

    Block(float x, float y, float z);

    void Draw(unsigned int);
};
