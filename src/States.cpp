#include "Objects.h"

// Functions for different starting states (rooms and ball configurations)

std::vector<Wall> cubeRoom(float wallLength) {
    std::vector<Wall> room;
    Wall bottomWall { { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f };
    room.push_back(bottomWall);
    Wall topWall { { 0.0f, wallLength, 0.0f }, 180.0f, 0.0f };
    room.push_back(topWall);
    Wall leftWall { { -wallLength / 2, wallLength / 2, 0.0f }, 90.0f, 90.0f };
    room.push_back(leftWall);
    Wall rightWall { { wallLength / 2, wallLength / 2, 0.0f }, 90.0f, -90.0f };
    room.push_back(rightWall);
    Wall southWall { { 0.0f, wallLength / 2, -wallLength / 2 }, 90.0f, 0.0f };
    room.push_back(southWall);
    Wall northWall { { 0.0f, wallLength / 2, wallLength / 2 }, 270.0f, 0.0f };
    room.push_back(northWall);
    return room;
}

std::vector<Ball3d> threeBallsBouncing() {
    std::vector<Ball3d> balls;
    Ball3d ball1 { Vector3 { -2.0f, 5.0f, 0.0f }, Vector3 { 0.02f, 0.5f, 0.0f } };
    balls.push_back(ball1);
    Ball3d ball2 { Vector3 { 2.0f, 5.0f, 0.0f }, Vector3 { -0.02f, -0.3f, 0.0f } };
    balls.push_back(ball2);
    Ball3d ball3 { { 0.0f, 1.5f, 1.5f }, { 0.001f, 0.001f, -0.01f } };
    ball3.color = RED;
    ball3.mass = 10;
    balls.push_back(ball3);
    return balls;
}