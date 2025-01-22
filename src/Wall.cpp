#include "Objects.h"
#include <cmath>

void Wall::set_xAxisRotation(float newXRotation) {
    xAxisRotation = newXRotation;
    xAxisRotationRadians = newXRotation * (PI / 180.0f);
}

void Wall::set_yAxisRotation(float newYRotation) {
    yAxisRotation = newYRotation;
    yAxisRotationRadians = newYRotation * (PI / 180.0f);
}

// unit vector normal to wall plane
Vector3 Wall::normalVector() {
    // calculation relies on normal vector initially lying along y-axis, then performing appropriate rotations
    return {sinf(yAxisRotationRadians) * sinf(xAxisRotationRadians), cosf(xAxisRotationRadians), cosf(yAxisRotationRadians) * sinf(xAxisRotationRadians)};
}

// unit vector lying in the wall plane
Vector3 Wall::inplaneVector() {
    // calculation relies on vector initially lying along z-axis, then performing appropriate rotations
    return {sinf(yAxisRotationRadians) * cosf(xAxisRotationRadians), -1.0f*sinf(xAxisRotationRadians), cosf(yAxisRotationRadians) * cosf(xAxisRotationRadians)};
}

float Wall::distanceToWall(Vector3 point) {
    float displacementAlongNormal = -1.0f * Vector3DotProduct(normalVector(), centerPosition);
    return abs(Vector3DotProduct(normalVector(), point) + displacementAlongNormal) / Vector3Length(normalVector());
}

void Wall::draw() {
    rlPushMatrix();
    rlTranslatef(centerPosition.x, centerPosition.y, centerPosition.z);
    rlRotatef(yAxisRotation, 0.0f, 1.0f, 0.0f);
    rlRotatef(xAxisRotation, 1.0f, 0.0f, 0.0f);
    // Draw wireframe like borders to make adjacent walls easier to see
    DrawLine3D( { -size.x / 2, 0.0f, size.y / 2 }, { size.x / 2, 0.0f, size.y / 2 }, BLACK );
    DrawLine3D( { -size.x / 2, 0.0f, -size.y / 2 }, { size.x / 2, 0.0f, -size.y / 2 }, BLACK );
    DrawLine3D( { size.x / 2, 0.0f, -size.y / 2 }, { size.x / 2, 0.0f, size.y / 2 }, BLACK );
    DrawLine3D( { -size.x / 2, 0.0f, -size.y / 2 }, { -size.x / 2, 0.0f, size.y / 2 }, BLACK );
    DrawPlane( { 0.0f, 0.0f, 0.0f }, size, wallColor );
    rlPopMatrix();

}