#ifndef COLLISION_GRID_H
#define COLLISION_GRID_H

#include "Objects.h"

struct GridCell {
    std::vector<int> ballIndices;
    void clearBalls() {
        ballIndices.clear();
    }
};

struct CollisionGrid {
    float cellSize;
    int numberCellsX;
    int numberCellsY;
    int numberCellsZ;
    Vector3 startingPosition;
    std::vector<GridCell> cells;  // Use a flattened grid for memory efficiency

    int getGridIndex(int x, int y, int z) {
        return x + y * numberCellsX + z * numberCellsX * numberCellsY;
    }

    void addBallToGrid(Ball3d &ball, int ballIndex) {
        Vector3 ballGridPostion = Vector3Subtract(ball.position, startingPosition);
        int cellIndex = getGridIndex(
            floor(ballGridPostion.x / cellSize), 
            floor(ballGridPostion.y / cellSize), 
            floor(ballGridPostion.z / cellSize)
        );
        cells[cellIndex].ballIndices.push_back(ballIndex);
    }
    
    void clearCells() {
        for (GridCell &cell : cells) {
            cell.clearBalls();
        }
    }

    CollisionGrid(float _cellSize, int nX, int nY, int nZ, Vector3 _startingPosition) :
        cellSize(_cellSize),
        numberCellsX(nX),
        numberCellsY(nY),
        numberCellsZ(nZ),
        startingPosition(_startingPosition),
        cells(numberCellsX * numberCellsY * numberCellsZ)
    {}
};

#endif // COLLISION_GRID_H
