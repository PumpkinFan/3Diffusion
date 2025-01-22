#include "Objects.h"

extern const float DT; 

Vector3 Ball3d::getVelocity() {
    return Vector3Subtract(position, pastPosition);
}

void Ball3d::updatePosition() {
    Vector3 velocity = getVelocity();
    pastPosition = position;
    position = Vector3Add(Vector3Add(position, Vector3Scale(velocity, DT)), Vector3Scale(acceleration, DT * DT));
}

void Ball3d::handleWallCollision(Wall wall) {
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

void Ball3d::draw() {
    DrawSphere(position, radius, color);
}

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
