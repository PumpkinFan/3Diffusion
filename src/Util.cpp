#include "Objects.h"

// float deltaTime() {return 1;}

float deltaTime() {
    float realDelta = GetFrameTime();
    if (realDelta == 0) {
        return 1.0f;
    }
    float expectedDelta = 1.0 / 35.0;  // based on 60fps, should refactor later
    return expectedDelta / realDelta; 
}
