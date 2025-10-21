#ifndef OBJS_H
#define OBJS_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <cmath>
#include <random>
#include <iostream>

const float DT = 1.0f;

// The Wall object is represented by a subset of the XZ plane that is rotated (around the x-axis then y-axis) and translated
struct Wall {
    Vector3 centerPosition;
    float xAxisRotation;
    float yAxisRotation;
    float xAxisRotationRadians = xAxisRotation * (PI / 180.0f);
    float yAxisRotationRadians = yAxisRotation * (PI / 180.0f);
    Vector2 size = { 10.0f, 10.0f };
    Color wallColor = { 0, 0, 0, 100 };

    void set_xAxisRotation(float newXRotation);
    void set_yAxisRotation(float newYRotation);

    Vector3 normalVector();
    Vector3 inplaneVector();
    float distanceToWall(Vector3 point);

    void draw();
};

struct Ball3d {
    Vector3 position;
    Vector3 initialVelocity;
    Vector3 pastPosition = Vector3Subtract(position, Vector3Scale(initialVelocity, DT));
    Vector3 acceleration = { 0.0f, 0.0f, 0.0f };
    float radius = 1.0f;
    int mass = 1;
    Color color = BLUE;
    bool trackPositions = false;
    std::vector<Vector3> previousPositions = {};

    Vector3 getVelocity();
    void setVelocity(Vector3 newVelocity);
    void updatePosition();
    void handleWallCollision(Wall &wall);

    void draw();
};

void handleBallCollision(Ball3d &ball1, Ball3d &ball2);

// create a cubic room with the bottom wall centered at the origin
std::vector<Wall> cubeRoom(float wallLength);
std::vector<Ball3d> threeBallsBouncing();
std::vector<Ball3d> brownianMotion(Vector3 roomDimensions, Ball3d &smallBall, Ball3d &largeBall, int nSmallBalls);
std::vector<Ball3d> generateBalls(Vector3 roomDimensions, float ballRadius, float velocityMagnitude, int numBalls);

#endif // OBJS_H
