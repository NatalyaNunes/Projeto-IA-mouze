#ifndef DIRECTION_HPP
#define DIRECTION_HPP

#include <vector>

/**
 * @brief Represents a position in the 2D grid.
 * 
 * Contains the coordinates `x` (row) and `y` (column) of a point.
 * Includes comparison operators for equality and ordering.
 */

struct Point{
    int x;
    int y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const Point& other) const {
    if (x < other.x) return true;
    if (x > other.x) return false;

    //quando o x for igual comparar o y
    return y < other.y;
    }
};

/**
 * @brief Direction enum for movement in the grid.
 * 
 * N: North (up), S: South (down), L: East (right), O: West (left).
 */

enum Dir { N, S, L, O };

/**
 * @brief Directional vectors for movement in the grid.
 * 
 * Each index corresponds to a Dir value (N, S, L, O) and contains
 * the relative position change for that direction.
 */

//Mover p direções usando as coordenadas do labirinto
const std::vector<Point> MOVES = {
    {-1, 0},  // N
    {1, 0},   // S
    {0, 1},   // L
    {0, -1}   // O
};

#endif