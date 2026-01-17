
#include "Camera.h"
#include <glad/glad.h>
#include <cmath>

Camera::Camera(float width, float height)
{
    float aspect = width / height;
    float fovDeg = 60.0f;           // ↓ smaller = less distortion
    float fovRad = fovDeg * 3.1415926f / 180.0f;

    float near = 0.1f;
    float far  = 100.0f;

    float f = 1.0f / tan(fovRad * 0.5f);

    projection[0]  = f / aspect;
    projection[1]  = 0;
    projection[2]  = 0;
    projection[3]  = 0;

    projection[4]  = 0;
    projection[5]  = f;
    projection[6]  = 0;
    projection[7]  = 0;

    projection[8]  = 0;
    projection[9]  = 0;
    projection[10] = (far + near) / (near - far);
    projection[11] = -1;

    projection[12] = 0;
    projection[13] = 0;
    projection[14] = (2 * far * near) / (near - far);
    projection[15] = 0;

    posX = posY = posZ = 0.0f;
    yaw   = -90.0f;   // look forward (OpenGL -Z)
    pitch = 0.0f;

    forwardX = 0.0f;
    forwardY = 0.0f;
    forwardZ = -1.0f;

    updateView();
}

void Camera::updateView()
{
    // World up
    const float worldUpX = 0.0f;
    const float worldUpY = 1.0f;
    const float worldUpZ = 0.0f;

    // Right = normalize(cross(forward, worldUp))
    rightX = forwardY * worldUpZ - forwardZ * worldUpY;
    rightY = forwardZ * worldUpX - forwardX * worldUpZ;
    rightZ = forwardX * worldUpY - forwardY * worldUpX;

    float rl = sqrt(rightX*rightX + rightY*rightY + rightZ*rightZ);
    rightX /= rl;
    rightY /= rl;
    rightZ /= rl;

    // Up = cross(right, forward)
    float upX = rightY * forwardZ - rightZ * forwardY;
    float upY = rightZ * forwardX - rightX * forwardZ;
    float upZ = rightX * forwardY - rightY * forwardX;

    // View matrix (column-major, OpenGL)
    view[0] = rightX;
    view[4] = rightY;
    view[8] = rightZ;
    view[12] = -(rightX*posX + rightY*posY + rightZ*posZ);

    view[1] = upX;
    view[5] = upY;
    view[9] = upZ;
    view[13] = -(upX*posX + upY*posY + upZ*posZ);

    view[2]  = -forwardX;
    view[6]  = -forwardY;
    view[10] = -forwardZ;
    view[14] = (forwardX*posX + forwardY*posY + forwardZ*posZ);

    view[3] = view[7] = view[11] = 0.0f;
    view[15] = 1.0f;
}


void Camera::upload(int shaderProgram)
{
    glUseProgram(shaderProgram);

    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
}

void Camera::MoveForward(float distance)
{
    posX += forwardX * distance;
    posY += forwardY * distance;
    posZ += forwardZ * distance;
    updateView();
}
void Camera::MoveRight(float distance)
{
    posX += rightX * distance;
    posY += rightY * distance;
    posZ += rightZ * distance;
    updateView();
}
void Camera::MoveUp(float distance)
{
    posY += distance ;
    updateView();
}

void Camera::ProcessMouse(float dx, float dy)
{
    float sensitivity = 0.1f;

    yaw   += dx * sensitivity;
    pitch -= dy * sensitivity; // inverted Y

    // Clamp pitch (avoid flipping)
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Convert to direction vector
    float radYaw   = yaw * 3.14159f / 180.0f;
    float radPitch = pitch * 3.14159f / 180.0f;

    forwardX = cos(radYaw) * cos(radPitch);
    forwardY = sin(radPitch);
    forwardZ = sin(radYaw) * cos(radPitch);

    // Normalize forward
    float len = sqrt(forwardX*forwardX + forwardY*forwardY + forwardZ*forwardZ);
    forwardX /= len;
    forwardY /= len;
    forwardZ /= len;


    updateView();
}

// GETTERS
float Camera::GetYaw()
{
    return yaw;
}
float Camera::GetPitch()
{
    return pitch;
}
