#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>

struct Vec2
{
    int x,y;
};
/* =========================================================
   CAMERA CLASS
   ========================================================= */
class Camera
{
public:
    double x, y, z;        // Position
    double yaw, pitch;     // Rotation
    double focalLength;    // Projection strength

    Camera()
        : x(0), y(0), z(0),
          yaw(0), pitch(0),
          focalLength(400.0) {}

    void moveForward(double amount)
    {
        x -= sin(yaw) * cos(pitch) * amount;
        y -= sin(pitch) * amount;
        z -= cos(yaw) * cos(pitch) * amount;
    }



    void moveRight(double amount)
    {
        x += cos(yaw) * amount;
        z -= sin(yaw) * amount;
    }
    void moveUp(double amount)
    {
        y += amount;
    }


};

/* =========================================================
   PROJECT A 3D POINT TO SCREEN
   ========================================================= */
bool projectPoint(
    const Camera& camera,
    double worldX, double worldY, double worldZ,
    int screenWidth, int screenHeight,
    int& screenX, int& screenY
)
{
    // Move point relative to camera
    double rx = worldX - camera.x;
    double ry = worldY - camera.y;
    double rz = worldZ - camera.z;

    // Rotate around Y axis (yaw)
    double cosYaw = cos(camera.yaw);
    double sinYaw = sin(camera.yaw);

    double x1 =  cosYaw * rx - sinYaw * rz;
    double z1 =  sinYaw * rx + cosYaw * rz;

    // Rotate around X axis (pitch)
    double cosPitch = cos(camera.pitch);
    double sinPitch = sin(camera.pitch);

    double y2 = cosPitch * ry - sinPitch * z1;
    double z2 = sinPitch * ry + cosPitch * z1;

    // Behind camera
    if (z2 >= 0)
        return false;

    // Perspective projection
    double projectedX = (x1 / -z2) * camera.focalLength;
    double projectedY = (y2 / -z2) * camera.focalLength;

    screenX = (int)(screenWidth  / 2 + projectedX);
    screenY = (int)(screenHeight / 2 - projectedY);

    return true;
}
void drawFilledTriangle2D(
    SDL_Renderer* renderer,
    Vec2 v0, Vec2 v1, Vec2 v2
)
{
    // Sort by y (v0.y <= v1.y <= v2.y)
    if (v1.y < v0.y) std::swap(v0, v1);
    if (v2.y < v0.y) std::swap(v0, v2);
    if (v2.y < v1.y) std::swap(v1, v2);

    auto drawFlatBottom = [&](Vec2 a, Vec2 b, Vec2 c)
    {
        float invSlope1 = (float)(b.x - a.x) / (b.y - a.y);
        float invSlope2 = (float)(c.x - a.x) / (c.y - a.y);

        float x1 = a.x;
        float x2 = a.x;

        for (int y = a.y; y <= b.y; y++)
        {
            SDL_RenderLine(renderer, (int)x1, y, (int)x2, y);
            x1 += invSlope1;
            x2 += invSlope2;
        }
    };

    auto drawFlatTop = [&](Vec2 a, Vec2 b, Vec2 c)
    {
        float invSlope1 = (float)(c.x - a.x) / (c.y - a.y);
        float invSlope2 = (float)(c.x - b.x) / (c.y - b.y);

        float x1 = c.x;
        float x2 = c.x;

        for (int y = c.y; y > a.y; y--)
        {
            SDL_RenderLine(renderer, (int)x1, y, (int)x2, y);
            x1 -= invSlope1;
            x2 -= invSlope2;
        }
    };

    // Case 1: flat bottom
    if (v1.y == v2.y)
    {
        drawFlatBottom(v0, v1, v2);
    }
    // Case 2: flat top
    else if (v0.y == v1.y)
    {
        drawFlatTop(v0, v1, v2);
    }
    // Case 3: split triangle
    else
    {
        float t = (float)(v1.y - v0.y) / (v2.y - v0.y);
        Vec2 vSplit =
        {
            (int)(v0.x + t * (v2.x - v0.x)),
            v1.y
        };

        drawFlatBottom(v0, v1, vSplit);
        drawFlatTop(v1, vSplit, v2);
    }
}
/* =========================================================
   DRAW 3D TRIANGLE
   ========================================================= */
void draw3DTriangle(
    SDL_Renderer* renderer,
    const Camera& camera,
    double ax, double ay, double az,
    double bx, double by, double bz,
    double cx, double cy, double cz,
    int w, int h
)
{
    Vec2 p0, p1, p2;

    // Project all three vertices
    if (!projectPoint(camera, ax, ay, az, w, h, p0.x, p0.y)) return;
    if (!projectPoint(camera, bx, by, bz, w, h, p1.x, p1.y)) return;
    if (!projectPoint(camera, cx, cy, cz, w, h, p2.x, p2.y)) return;
    drawFilledTriangle2D(renderer, p0, p1, p2);
}

/* =========================================================
   DRAW 3D LINE
   ========================================================= */
void draw3DLine(
    SDL_Renderer* renderer,
    const Camera& camera,
    double ax, double ay, double az,
    double bx, double by, double bz,
    int w, int h
)
{
    int x1, y1, x2, y2;

    if (projectPoint(camera, ax, ay, az, w, h, x1, y1) &&
            projectPoint(camera, bx, by, bz, w, h, x2, y2))
    {
        SDL_RenderLine(renderer, x1, y1, x2, y2);
    }
}

/* =========================================================
   DRAW A CUBE
   ========================================================= */
void drawCube(
    SDL_Renderer* renderer,
    const Camera& camera,
    double cx, double cy, double cz,
    double size,
    int w, int h
)
{
    double s = size / 2.0;

    double v[8][3] =
    {
        {cx-s, cy-s, cz-s}, {cx+s, cy-s, cz-s},
        {cx+s, cy+s, cz-s}, {cx-s, cy+s, cz-s},
        {cx-s, cy-s, cz+s}, {cx+s, cy-s, cz+s},
        {cx+s, cy+s, cz+s}, {cx-s, cy+s, cz+s}
    };

    int edges[12][2] =
    {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
    int triangles[12][3] =
    {
        // Back face (−Z)
        {0, 1, 2}, {0, 2, 3},

        // Front face (+Z)
        {4, 6, 5}, {4, 7, 6},

        // Left face (−X)
        {0, 3, 7}, {0, 7, 4},

        // Right face (+X)
        {1, 5, 6}, {1, 6, 2},

        // Bottom face (−Y)
        {0, 4, 5}, {0, 5, 1},

        // Top face (+Y)
        {3, 2, 6}, {3, 6, 7}
    };

    for (int i = 0; i < 12; i++)
    {
        SDL_SetRenderDrawColor(renderer,21*i,5*i,i,255);
        draw3DLine(
            renderer, camera,
            v[edges[i][0]][0], v[edges[i][0]][1], v[edges[i][0]][2],
            v[edges[i][1]][0], v[edges[i][1]][1], v[edges[i][1]][2],
            w, h
        );

        draw3DTriangle(renderer,camera,
                       v[triangles[i][0]][0], v[triangles[i][0]][1], v[triangles[i][0]][2],
                       v[triangles[i][1]][0], v[triangles[i][1]][1], v[triangles[i][1]][2],
                       v[triangles[i][2]][0], v[triangles[i][2]][1], v[triangles[i][2]][2],w,h);
    }

}

/* =========================================================
   MAIN
   ========================================================= */
int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
                             "Software 3D Camera",
                             800, 600,
                             SDL_WINDOW_RESIZABLE
                         );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetWindowRelativeMouseMode(window, true);

    Camera camera;

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                    running = false;

                if (event.key.key == SDLK_Z) camera.moveForward( 0.15);
                if (event.key.key == SDLK_S) camera.moveForward(-0.15);
                if (event.key.key == SDLK_Q) camera.moveRight( -0.15);
                if (event.key.key == SDLK_D) camera.moveRight(  0.15);
                if (event.key.key == SDLK_SPACE) camera.moveUp(0.15);
                if (event.key.key == SDLK_LSHIFT) camera.moveUp(-0.15);
                if (event.key.key == SDLK_H)
                {
                    std::cout << "yaw: " <<camera.yaw << " pitch: " << camera.pitch << "\n";
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                camera.yaw   -= event.motion.xrel * 0.002;
                camera.pitch += event.motion.yrel * 0.002;

                if (camera.pitch >  1.55) camera.pitch =  1.55;
                if (camera.pitch < -1.55) camera.pitch = -1.55;
            }
        }

        int w, h;
        SDL_GetRenderOutputSize(renderer, &w, &h);
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255,255, 255);
        for(int i = -20; i <20 ; i++)
        {
            for(int j = -20; j <20 ; j++)
            {
                if(j == -20){
                    SDL_SetRenderDrawColor(renderer,0,255,0,255);
                }
                else{
                    SDL_SetRenderDrawColor(renderer,255,255,255,255);
                }
                draw3DLine(renderer, camera, i, -2, j, i+1, -2, j, w, h);
                draw3DLine(renderer, camera, i, -2, j, i, -2, j+1, w, h);
            }
        }
        drawCube(renderer, camera, 0, 0, -5, 2.0, w, h);
        SDL_RenderPresent(renderer);
    }

    SDL_Quit();
    return 0;
}
