#include <iostream>
#include <vector>
#include "math.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

const float DT = 1;
const Color INNER_WALL_COLOR = { 0, 0, 0, 100 };
const Color OUTER_WALL_COLOR = { 255, 255, 255, 255 };

// The idea 
// Define a `Wall` object
// This essentially represents a subsection of a rotated plane
struct Wall {
    Vector3 centerPosition;
    float xAxisRotation;
    float yAxisRotation;
    float xAxisRotationRadians = xAxisRotation * (PI / 180.0f);
    float yAxisRotationRadians = yAxisRotation * (PI / 180.0f);
    Vector2 size = { 10.0f, 10.0f };

    void set_xAxisRotation(float newXRotation) {
        xAxisRotation = newXRotation;
        xAxisRotationRadians = newXRotation * (PI / 180.0f);
    }

    void set_yAxisRotation(float newYRotation) {
        yAxisRotation = newYRotation;
        yAxisRotationRadians = newYRotation * (PI / 180.0f);
    }

    // unit vector normal to wall plane
    Vector3 normalVector() {
        // calculation relies on normal vector initially lying along y-axis, then performing appropriate rotations
        return {sin(yAxisRotationRadians) * sin(xAxisRotationRadians), cos(xAxisRotationRadians), cos(yAxisRotationRadians) * sin(xAxisRotationRadians)};
    }

    // unit vector lying in the wall plane
    Vector3 inplaneVector() {
        // calculation relies on vector initially lying along z-axis, then performing appropriate rotations
        return {sin(yAxisRotationRadians) * cos(xAxisRotationRadians), -1.0f*sin(xAxisRotationRadians), cos(yAxisRotationRadians) * cos(xAxisRotationRadians)};
    }

    float distanceToWall(Vector3 point) {
        float displacementAlongNormal = -1.0f * Vector3DotProduct(normalVector(), centerPosition);
        return abs(Vector3DotProduct(normalVector(), point) + displacementAlongNormal) / Vector3Length(normalVector());
    }

    void draw() {
        rlPushMatrix();
        rlTranslatef(centerPosition.x, centerPosition.y, centerPosition.z);
        rlRotatef(yAxisRotation, 0.0f, 1.0f, 0.0f);
        rlRotatef(xAxisRotation, 1.0f, 0.0f, 0.0f);
        // Draw wireframe like borders to make adjacent walls easier to see
        DrawLine3D( { -size.x / 2, 0.0f, size.y / 2 }, { size.x / 2, 0.0f, size.y / 2 }, BLACK );
        DrawLine3D( { -size.x / 2, 0.0f, -size.y / 2 }, { size.x / 2, 0.0f, -size.y / 2 }, BLACK );
        DrawLine3D( { size.x / 2, 0.0f, -size.y / 2 }, { size.x / 2, 0.0f, size.y / 2 }, BLACK );
        DrawLine3D( { -size.x / 2, 0.0f, -size.y / 2 }, { -size.x / 2, 0.0f, size.y / 2 }, BLACK );
        DrawPlane( { 0.0f, 0.0f, 0.0f }, size, INNER_WALL_COLOR );
        rlPopMatrix();

        // Draw back side of the wall
        // Vector3 _inplaneVector = inplaneVector();
        // rlPushMatrix();
        // rlTranslatef(centerPosition.x, centerPosition.y, centerPosition.z);
        // rlRotatef(180.0f, _inplaneVector.x, _inplaneVector.y, _inplaneVector.z);
        // rlRotatef(yAxisRotation, 0.0f, 1.0f, 0.0f);
        // rlRotatef(xAxisRotation, 1.0f, 0.0f, 0.0f);
        // DrawPlane(Vector3 { 0.0f, 0.0f, 0.0f }, size, OUTER_WALL_COLOR );
        // rlPopMatrix();
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
            Vector3 normalVector = wall.normalVector();
            float normalVelocityMagnitude = Vector3DotProduct(velocity, normalVector);
            Vector3 normalVelocity = Vector3Scale(normalVector, normalVelocityMagnitude);

            // we can reverse the velocity in the normal direction by subtracting two times its component in that direction
            Vector3 newVelocity = Vector3Add(velocity, Vector3Scale(normalVelocity, -2.0f));

            // resolve overlap after collision
            Vector3 nextPosition = Vector3Add(position, Vector3Scale(newVelocity, DT));
            float overlapAfterCollision = radius - wall.distanceToWall(nextPosition);
            if (overlapAfterCollision > 0.0f) {
                position = Vector3Add(position, Vector3Scale(normalVector, overlapAfterCollision));
            }

            pastPosition = Vector3Subtract(position, Vector3Scale(newVelocity, DT));
        }
    }

    void draw() {
        DrawSphere(position, radius, color);
    }
};

void handleBallCollision(Ball3d &ball1, Ball3d &ball2) {
    float distance = Vector3Distance(ball1.position, ball2.position);
    if (distance > (ball1.radius + ball2.radius)) {
        return;  // No collision
    }

    Vector3 velocity1 = ball1.getVelocity();
    Vector3 velocity2 = ball2.getVelocity();

    // Find unit normal vector
    Vector3 normalVector = Vector3Subtract(ball1.position, ball2.position);  // points towards ball1
    Vector3 unitNormal = Vector3Scale(normalVector, 1.0f / distance);

    // Project velocities onto normal direction
    float normalVelocity1 = Vector3DotProduct(unitNormal, velocity1);
    float normalVelocity2 = Vector3DotProduct(unitNormal, velocity2);

    // Perform 1D elastic collision on normal components
    float massSum = ball1.mass + ball2.mass;
    float newNormalVelocity1 = (normalVelocity1 * (ball1.mass - ball2.mass) + 2 * ball2.mass * normalVelocity2) / massSum;
    float newNormalVelocity2 = (normalVelocity2 * (ball2.mass - ball1.mass) + 2 * ball1.mass * normalVelocity1) / massSum;

    // Create new velocity vectors (subtract the current normal velocity and add the new velocity)
    Vector3 newVelocity1 = Vector3Add(velocity1, Vector3Scale(unitNormal, newNormalVelocity1 - normalVelocity1));
    Vector3 newVelocity2 = Vector3Add(velocity2, Vector3Scale(unitNormal, newNormalVelocity2 - normalVelocity2));

    // Resolve overlap after collision
    float radiusSum = ball1.radius + ball2.radius;
    float distanceAfterCollision = Vector3Distance(Vector3Add(ball1.position, Vector3Scale(newVelocity1, DT)),
                                                   Vector3Add(ball2.position, Vector3Scale(newVelocity2, DT)));
    float overlapAfterCollision = radiusSum - distanceAfterCollision;
    if (overlapAfterCollision > 0) {
        ball1.position = Vector3Add(ball1.position, Vector3Scale(unitNormal, overlapAfterCollision * (ball1.radius / radiusSum)));
        ball2.position = Vector3Add(ball2.position, Vector3Scale(unitNormal, -1.0f * overlapAfterCollision * (ball2.radius / radiusSum)));
    }

    ball1.pastPosition = Vector3Subtract(ball1.position, Vector3Scale(newVelocity1, DT));
    ball2.pastPosition = Vector3Subtract(ball2.position, Vector3Scale(newVelocity2, DT));
} 


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
    Wall topWall { { 0.0f, 10.0f, 0.0f }, 180.0f, 0.0f };
    room.push_back(topWall);
    Wall leftWall { { -5.0f, 5.0f, 0.0f }, 90.0f, 90.0f };
    room.push_back(leftWall);
    Wall rightWall { { 5.0f, 5.0f, 0.0f }, 90.0f, -90.0f };
    room.push_back(rightWall);
    Wall southWall { { 0.0f, 5.0f, -5.0f }, 90.0f, 0.0f };
    room.push_back(southWall);
    Wall northWall { { 0.0f, 5.0f, 5.0f }, 270.0f, 0.0f };
    room.push_back(northWall);

    std::vector<Ball3d> balls;
    Ball3d ball1 { Vector3 { -2.0f, 5.0f, 0.0f }, Vector3 { 0.02f, 0.5f, 0.0f } };
    balls.push_back(ball1);
    Ball3d ball2 { Vector3 { 2.0f, 5.0f, 0.0f }, Vector3 { -0.02f, -0.3f, 0.0f } };
    balls.push_back(ball2);
    Ball3d ball3 { { 0.0f, 1.5f, 1.5f }, { 0.001f, 0.001f, -0.01f } };
    ball3.color = RED;
    ball3.mass = 10;
    balls.push_back(ball3);

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