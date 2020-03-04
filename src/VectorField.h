#pragma once

#include "Grid.hpp"
#include "StarcraftMap.hpp"

#include <vector>

struct Direction
{
    char x = 0;
    char y = 0;
};

struct Cell { int x = 0; int y = 0; };

class VectorField
{
    StarcraftMap * m_map = nullptr;
    //Grid<short> m_grid;
    Grid<short> m_distance;
    Grid<unsigned char> m_directions;

    // SOLUTION STUFF
    int m_gx = -1;   // position of the goal location
    int m_gy = -1;
    const std::vector<Direction> m_actions = { {0,1}, {0,-1}, {1,0}, {-1,0} };
    const std::vector<int> m_oppositeActions = { 1, 0, 3, 2 };
    std::vector<Cell> m_stack;

    void computeDistance();
    bool validCell(int x, int y) const;

public:

    VectorField();

    // called by the GUI to compute the grid, distance map, and vector field
    void compute(StarcraftMap * map, size_t gx, size_t gy);

    // returns the value of the blocked grid (0 = unblocked, 1 = blocked)
    int getGrid(size_t x, size_t y) const;

    // returns the value of the disance map at cell (x,y)
    int getDistance(size_t x, size_t y) const;

    // returns the directions (vectors) of the vector field at cell (x,y)
    size_t getNumDirections(size_t x, size_t y) const;
    Direction getDirection(size_t x, size_t y, size_t index) const;

    inline size_t width() const { return m_map ? m_map->width() : 0; }
    inline size_t height() const { return m_map ? m_map->height() : 0; }
};