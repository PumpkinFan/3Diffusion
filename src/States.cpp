#include "Objects.h"

// Functions for different starting states (rooms and ball configurations)

std::vector<Wall> cubeRoom(float wallLength) {
    std::vector<Wall> room;
    Wall bottomWall { { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f };
    bottomWall.size = { wallLength, wallLength };
    room.push_back(bottomWall);
    Wall topWall { { 0.0f, wallLength, 0.0f }, 180.0f, 0.0f };
    topWall.size = { wallLength, wallLength };
    room.push_back(topWall);

    Wall leftWall { { -wallLength / 2, wallLength / 2, 0.0f }, 90.0f, 90.0f };
    leftWall.size = { wallLength, wallLength };
    room.push_back(leftWall);
    Wall rightWall { { wallLength / 2, wallLength / 2, 0.0f }, 90.0f, -90.0f };
    rightWall.size = { wallLength, wallLength };
    room.push_back(rightWall);
    
    Wall southWall { { 0.0f, wallLength / 2, -wallLength / 2 }, 90.0f, 0.0f };
    southWall.size = { wallLength, wallLength };
    room.push_back(southWall);
    Wall northWall { { 0.0f, wallLength / 2, wallLength / 2 }, 270.0f, 0.0f };
    northWall.size = { wallLength, wallLength };
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

// Return vector containing largeBall and copies of smallBall to fill the room
// smallBall copies will have their velocity vector randomly rotated and color slightly randomized
std::vector<Ball3d> brownianMotion(Vector3 roomDimensions, Ball3d &smallBall, Ball3d &largeBall, int nSmallBalls) {
    std::vector<Ball3d> balls;

    balls.push_back(largeBall);

    // Calculate spacing between balls
    float spacingX = (roomDimensions.x - 2 * smallBall.radius) / std::cbrt(nSmallBalls);
    float spacingY = (roomDimensions.y - 2 * smallBall.radius) / std::cbrt(nSmallBalls);
    float spacingZ = (roomDimensions.z - 2 * smallBall.radius) / std::cbrt(nSmallBalls);
    
    int ballsPerRow = std::cbrt(nSmallBalls);

    // Random number generator for velocity direction and color modification
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::uniform_int_distribution<int> redDist(-10, 10);
    std::uniform_int_distribution<int> greenDist(-10, 10);
    std::uniform_int_distribution<int> blueDist(-10, 10);

    for (int i = 0; i < ballsPerRow; ++i) {
        for (int j = 0; j < ballsPerRow; ++j) {
            for (int k = 0; k < ballsPerRow; ++k) {
                if (balls.size() >= nSmallBalls) {break;}

                // Calculate ball position
                Vector3 ballPosition = {
                    -roomDimensions.x / 2 + smallBall.radius + i * spacingX,
                    smallBall.radius + j * spacingY,
                    -roomDimensions.z / 2 + smallBall.radius + k * spacingZ
                };
                
                // If it's inside the largeBall don't add it
                if (Vector3Distance(ballPosition, largeBall.position) < (smallBall.radius + largeBall.radius)) {
                    continue;
                }

                // Randomize the velocity
                Vector3 randomDirection = Vector3Normalize({ dist(gen), dist(gen), dist(gen) });
                float velocityMagnitude = Vector3Length(smallBall.getVelocity());
                Vector3 ballVelocity = Vector3Scale(randomDirection, velocityMagnitude);

                // Create the small ball copy
                Ball3d ball {ballPosition, ballVelocity};
                ball.radius = smallBall.radius;
                ball.color = smallBall.color;
                ball.mass = smallBall.mass;

                balls.push_back(ball);

            }
        }
    }
    return balls;
}

std::vector<Ball3d> generateBalls(Vector3 roomDimensions, float ballRadius, float velocityMagnitude, int numBalls) {
    std::vector<Ball3d> balls;

    // Calculate spacing between balls
    float spacingX = (roomDimensions.x - 2 * ballRadius) / std::cbrt(numBalls);
    float spacingY = (roomDimensions.y - 2 * ballRadius) / std::cbrt(numBalls);
    float spacingZ = (roomDimensions.z - 2 * ballRadius) / std::cbrt(numBalls);

    int ballsPerRow = std::cbrt(numBalls);

    // Random number generator for velocity direction
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::uniform_int_distribution<int> redDist(0, 50);
    std::uniform_int_distribution<int> greenDist(0, 50);
    std::uniform_int_distribution<int> blueDist(100, 255);

    for (int i = 0; i < ballsPerRow; ++i) {
        for (int j = 0; j < ballsPerRow; ++j) {
            for (int k = 0; k < ballsPerRow; ++k) {
                if (balls.size() >= numBalls) break;

                // Calculate ball position
                Vector3 position = {
                    -roomDimensions.x / 2 + ballRadius + i * spacingX,
                    ballRadius + j * spacingY,
                    -roomDimensions.z / 2 + ballRadius + k * spacingZ
                };

                // std::cout << position.x << ", " << position.y  << ", " << position.z << "\n";

                // Generate a random velocity direction
                Vector3 randomDirection = { dist(gen), dist(gen), dist(gen) };
                randomDirection = Vector3Normalize(randomDirection);

                // Assign velocity based on magnitude
                Vector3 initialVelocity = Vector3Scale(randomDirection, velocityMagnitude);

                // Create and add the ball
                Ball3d ball;
                ball.position = position;
                ball.initialVelocity = initialVelocity;
                ball.setVelocity(initialVelocity);
                ball.radius = ballRadius;

                ball.color = Color { static_cast<unsigned char>(redDist(gen)), 
                                     static_cast<unsigned char>(greenDist(gen)), 
                                     static_cast<unsigned char>(blueDist(gen)), 
                                     255 };

                balls.push_back(ball);
            }
        }
    }

    return balls;
}