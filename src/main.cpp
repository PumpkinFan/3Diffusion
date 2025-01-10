#include <iostream>
#include <vector>
#include "math.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

const float DT = 1;

// The idea 
// Define a `Wall` object
// This essentially represents a subsection of a rotated plane
struct Wall {
    Vector3 centerPosition;
    float xAxisRotation;
    float yAxisRotation;
    Vector2 size = { 10.0f, 10.0f };

    // unit vector normal to wall plane
    Vector3 normalVector() {
        // calculation relies on normal vector initially lying along y-axis, then performing appropriate rotations
        float xAxisRotationRadians = xAxisRotation * (PI / 180.0f);
        float yAxisRotationRadians = yAxisRotation * (PI / 180.0f);
        return {sin(yAxisRotationRadians) * cos(xAxisRotationRadians), cos(xAxisRotationRadians), cos(yAxisRotationRadians) * sin(xAxisRotationRadians)};
    }

    float distanceToWall(Vector3 point) {
        float displacementAlongNormal = -1.0f * Vector3DotProduct(normalVector(), centerPosition);
        return abs(Vector3DotProduct(normalVector(), point) + displacementAlongNormal) / Vector3Length(normalVector());
    }

    void draw() {
        rlPushMatrix();
        rlTranslatef(centerPosition.x, centerPosition.y, centerPosition.z);
        rlRotatef(xAxisRotation, 1.0f, 0.0f, 0.0f);
        rlRotatef(yAxisRotation, 0.0f, 1.0f, 0.0f);
        DrawPlane( { 0.0f, 0.0f, 0.0f }, size, BLACK);
        // DrawLine3D( { 0.0f, 0.0f, 0.0f }, normalVector(), RED);
        rlPopMatrix();

        // Draw back side of the wall
        rlPushMatrix();
        rlTranslatef(centerPosition.x, centerPosition.y, centerPosition.z);
        rlRotatef(xAxisRotation + 180.0f, 1.0f, 0.0f, 0.0f);
        rlRotatef(yAxisRotation + 180.0f, 0.0f, 1.0f, 0.0f);
        DrawPlane(Vector3 { 0.0f, 0.0f, 0.0f }, size, Color { 125, 125, 125, 200 });
        rlPopMatrix();
    }
    
};

struct Ball3d {
    Vector3 position;
    Vector3 initialVelocity;
    Vector3 pastPosition = Vector3Subtract(position, Vector3Scale(initialVelocity, DT));
    Vector3 acceleration = { 0.0f, 0.0f, 0.0f };
    float radius = 1.0f;
    int mass = 1;
    Color color = BLUE;

    Vector3 getVelocity() {
        return Vector3Subtract(position, pastPosition);
    }

    void updatePosition() {
        Vector3 velocity = getVelocity();
        pastPosition = position;
        position = Vector3Add(Vector3Add(position, Vector3Scale(velocity, DT)), Vector3Scale(acceleration, DT * DT));
    }

    void handleWallCollision(Wall wall) {
        if (wall.distanceToWall(position) <= radius) {
            Vector3 velocity = getVelocity();

            // We can do this to get the normal component because the normalVector is a unit vector
            float normalVelocityMagnitude = Vector3DotProduct(velocity, wall.normalVector());
            Vector3 normalVelocity = Vector3Scale(wall.normalVector(), normalVelocityMagnitude);

            // we can reverse the velocity in the normal direction by subtracting two times its component in that direction
            Vector3 newVelocity = Vector3Add(velocity, Vector3Scale(normalVelocity, -2.0f));

            pastPosition = Vector3Subtract(position, Vector3Scale(newVelocity, DT));
        }
    }

    void draw() {
        DrawSphere(position, radius, color);
    }
};




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

    std::vector<Wall> room;
    Wall bottomWall { { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f };
    room.push_back(bottomWall);    
    Wall topWall { { 0.0f, 10.0f, 0.0f}, 180.0f, 0.0f };
    room.push_back(topWall);

    Ball3d ball { Vector3 { 0.0f, 5.0f, 0.0f }, Vector3 { 0.0f, -0.05f, 0.0f } };

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FREE);

        for (Wall wall : room) {
            std::cout << wall.distanceToWall(ball.position) << "    ";
            ball.handleWallCollision(wall);
        }
        std::cout << std::endl;
        ball.updatePosition();

        if (IsKeyPressed('Z')) camera.target = Vector3 { 0.0f, 0.0f, 0.0f };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
     
                ball.draw();
                for (Wall wall : room) {
                    wall.draw();
                }

                DrawSphere({ 0.0f, 0.0f, 0.0f }, 0.1f, BLUE);

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