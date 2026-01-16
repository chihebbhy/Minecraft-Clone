#pragma once
#include <SDL3/SDL.h>

class Camera;

class Input
{
public:
    static bool HandleEvent(const SDL_Event& e, Camera& cam);
};
