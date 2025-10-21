#include <iostream>
#include <vector>
#include <string>
#include "raylib.h"
#include "raymath.h"

#include "Objects.h"
#include "CollisionGrid.h"

const float updateDelta = 1.0f / 30.0f;  // update the simulation every 30th of a second

// TODO:
// Add path tracking for large particle
// Add pausing and resetting simulation
// Use grid for more efficient collision detection


int main() {

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
    bool simulationPaused = false;
    bool drawFrameRate = false;

    InitWindow(screenWidth, screenHeight, "3Diffusion");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = Vector3 { 20.0f, 20.0f, 20.0f }; // Camera position
    camera.target = Vector3 { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3 { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    float roomSize = 20.0f;

    std::vector<Wall> room = cubeRoom(roomSize);

    Ball3d smallBall = {{0.0f, 0.0f, 0.0f}, {0.3f, 0.0f, 0.0f}};
    smallBall.radius = 0.5f;
    smallBall.color = BLUE;
    smallBall.mass = 4; 

    Ball3d largeBall = {{0.0f, 10.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
    largeBall.radius = 2.0f;
    largeBall.color = RED;
    largeBall.mass = 64;
    // largeBall.trackPositions = true;


    Vector3 roomDimensions = { roomSize, roomSize, roomSize };
    std::vector<Ball3d> balls = brownianMotion(roomDimensions, smallBall, largeBall, 300);
    balls[0].trackPositions = true;
    // std::vector<Ball3d> balls = generateBalls(roomDimensions, 0.5f, 0.3f, 300);
    // std::vector<Ball3d> balls = threeBallsBouncing();
    // balls.push_back(largeBall);

    // Create grid for more efficient collision detection
    CollisionGrid grid = { 5.0f, 20, 20, 20, { -0.5f * roomSize, 0.0f, -0.5f * roomSize } };

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Use accumulator to track time change between frames and only update physics after updateDelta amount of time
    float timeAccumulator = 0.0f;

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FREE);

        if (!simulationPaused) {
            timeAccumulator += GetFrameTime();

            while (timeAccumulator >= updateDelta) {
                // Update physics
                for (int i = 0; i < balls.size(); ++i) {
                    for (Wall wall : room) {
                        balls[i].handleWallCollision(wall);
                    }
                    for (int j = i + 1; j < balls.size(); ++j) {
                        handleBallCollision(balls[i], balls[j]);
                    }
                    balls[i].updatePosition();
                }
                // grid.clearCells();
                // for (int i = 0; i < balls.size(); ++i) {
                //     for (Wall wall : room) {
                //         balls[i].handleWallCollision(wall);
                //     }
                //     grid.addBallToGrid(balls[i], i);
                //     for (int j = i + 1; j < balls.size(); ++j) {
                //         handleBallCollision(balls[i], balls[j]);
                //     }
                //     balls[i].updatePosition();
                // }
                // for (GridCell &cell : grid.cells) {
                //     for (int i = 0; i < cell.ballIndices.size(); ++i) {
                //         int ballIndex = cell.ballIndices[i];
                //         for (int j = i + 1; j < cell.ballIndices.size(); ++j) {
                //             int otherBallIndex = cell.ballIndices[j];
                //             handleBallCollision(balls[ballIndex], balls[otherBallIndex]);
                //         }
                //         balls[ballIndex].updatePosition();
                //     }
                // }
                timeAccumulator -= updateDelta;
            }
        }

        // User inputs
        //----------------------------------------------------------------------------------
        if (IsKeyPressed('Z')) camera.target = Vector3 { 0.0f, 0.0f, 0.0f };
        if (IsKeyPressed('X')) simulationPaused = !simulationPaused;
        if (IsKeyPressed(KEY_F2)) drawFrameRate = !drawFrameRate;

        // reset the simulation
        if (IsKeyPressed('R') && IsKeyDown(KEY_LEFT_ALT)) {
            balls = brownianMotion(roomDimensions, smallBall, largeBall, 300);
        }

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
                std::vector<Vector3> largeBallPositions = balls[0].previousPositions;
                for (int i = 0; i < largeBallPositions.size(); ++i) {
                    if (i < largeBallPositions.size() - 1) {  // Exclude most recent position
                        // std::cout << largeBall.previousPositions[i].x  << ", " << largeBall.previousPositions[i].y << ", " << largeBall.previousPositions[i].z << std::endl;
                        DrawLine3D(largeBallPositions[i], largeBallPositions[i+1], BLACK);
                    }
                }

                // Draw the origin (optional)
                // DrawSphere({ 0.0f, 0.0f, 0.0f }, 0.1f, LIME);

            EndMode3D();

            DrawRectangle( 10, 10, 320, 133, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 133, BLUE);

            DrawText("Controls:", 20, 20, 10, BLACK);
            DrawText("- WASD + SPACE to move around the camera", 40, 40, 10, DARKGRAY);
            DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, DARKGRAY);
            DrawText("- Mouse Wheel Pressed to Pan", 40, 80, 10, DARKGRAY);
            DrawText("- Z to zoom to (0, 0, 0)", 40, 100, 10, DARKGRAY);
            DrawText("- X to pause/resume simulation", 40, 120, 10, DARKGRAY);

            if (drawFrameRate) {
                int fps = GetFPS();
                std::string fpsString = std::to_string(fps);
                DrawText(fpsString.c_str(), screenWidth - 100, 20, 50, LIME);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;

}