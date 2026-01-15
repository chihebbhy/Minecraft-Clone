#pragma once

class Camera
{
public:
    Camera(float width, float height);

    void upload(int shaderProgram);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void ProcessMouse(float dx, float dy);
    float GetYaw();
    float GetPitch();
private:
    void updateView();
    float projection[16];
    float view[16];

    // position
    float posX,posY,posZ;

    // rotation
    float yaw,pitch;

    float forwardX, forwardY, forwardZ;
    float rightX, rightY, rightZ;
};
