#include <iostream>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#include "Objects.h"

// TODO:
// Refactor DT to use GetFrameTime
// Refactor brownianMotion function
// Add path tracking for large particle

int main() {

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "3Diffusion");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = Vector3 { 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = Vector3 { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3 { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    std::vector<Wall> room = cubeRoom(20.0f);

    Ball3d smallBall = {{0.0f, 0.0f, 0.0f}, {0.3f, 0.0f, 0.0f}};
    smallBall.radius = 0.5f;
    smallBall.color = BLUE;
    smallBall.mass = 4; 

    Ball3d largeBall = {{0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    largeBall.radius = 2.0f;
    largeBall.color = RED;
    largeBall.mass = 64;


    Vector3 roomDimensions = { 20.0f, 20.0f, 20.0f };
    std::vector<Ball3d> balls = brownianMotion(roomDimensions, smallBall, largeBall, 300);
    // std::vector<Ball3d> balls = generateBalls(roomDimensions, 0.5f, 0.3f, 300);
    // balls.push_back(largeBall);

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FREE);


        for (int i = 0; i < balls.size(); ++i) {
            for (Wall wall : room) {
                balls[i].handleWallCollision(wall);
            }
            for (int j = i + 1; j < balls.size(); ++j) {
                handleBallCollision(balls[i], balls[j]);
            }
            balls[i].updatePosition();
        }

        if (IsKeyPressed('Z')) camera.target = Vector3 { 0.0f, 0.0f, 0.0f };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
     
                for (Ball3d ball : balls) {
                    ball.draw();
                }
                for (Wall wall : room) {
                    wall.draw();
                }

                // Draw the origin (optional)
                // DrawSphere({ 0.0f, 0.0f, 0.0f }, 0.1f, BLUE);

            EndMode3D();

            DrawRectangle( 10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 93, BLUE);

            DrawText("Free camera default controls:", 20, 20, 10, BLACK);
            DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
            DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, DARKGRAY);
            DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;

}