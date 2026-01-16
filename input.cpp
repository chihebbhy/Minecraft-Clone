
#include "Input.h"
#include "Camera.h"
#include <iostream>

bool Input::HandleEvent(const SDL_Event& e, Camera& Cam)
{
    switch(e.type)
    {
    case SDL_EVENT_QUIT:
        return false;
    case SDL_EVENT_KEY_DOWN:
        switch(e.key.key)
        {
        case SDLK_ESCAPE:
            return false;
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
    return true;
}

