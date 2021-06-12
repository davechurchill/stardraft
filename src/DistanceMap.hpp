#pragma once

#include "Grid2D.hpp"
#include "StarDraftMap.hpp"

#include <vector>

class DistanceMap
{
    const StarDraftMap *    m_map           = nullptr;
    int                     m_gx            = -1;
    int                     m_gy            = -1;
    Grid2D<short>           m_distance;
    Grid2D<unsigned char>   m_directions;
    std::vector<Tile>       m_stack;

    std::vector<Tile>       m_actions         = { {0,1}, {0,-1}, {1,0}, {-1,0} };
    std::vector<int>        m_oppositeActions = { 1, 0, 3, 2 };

    void computeDistance()
    {
        m_distance = Grid2D<short>(m_map->width(), m_map->height(), -1);
        m_directions = Grid2D<unsigned char>(m_map->width(), m_map->height(), {});
        if (!validCell(m_gx, m_gy)) { return; }

        m_stack.clear();    

        m_stack.push_back({ m_gx, m_gy });
        m_distance.set(m_gx, m_gy, 0);

        // iterate until the current pointer meets the end
        for (size_t i = 0; i < m_stack.size(); i++)
        {
            // get a copy of the cell and its distance to avoid future lookups
            Tile cell = m_stack[i];
            int dist = m_distance.get(cell.x, cell.y);

            for (size_t a = 0; a < m_actions.size(); a++)
            {
                // calculate the child cell
                Tile next = { cell.x + m_actions[a].x, cell.y + m_actions[a].y };

                // if this child is a valid cell we can do stuff with it
                if (validCell(next.x, next.y))
                {
                    // save the distance to the next cell to avoid another lookup
                    auto ndist = m_distance.get(next.x, next.y);

                    // if we haven't seen this cell before
                    if (ndist == -1)
                    {
                        // add it to the stack and set the distance
                        m_stack.emplace_back(next);
                        m_distance.set(next.x, next.y, dist + 1);

                        // point the action back at the parent
                        m_directions.get(next.x, next.y) |= (1 << m_oppositeActions[a]);
                    }
                    // if we have seen it before but it has the same distance to parent
                    else if (ndist == dist + 1)
                    {
                        // then we can also point back at this node
                        m_directions.get(next.x, next.y) |= (1 << m_oppositeActions[a]);
                    }
                }
            }
        }
    }

    bool validCell(int x, int y) const
    {
        return (x >= 0 && x < (int)width() && y >= 0 && y < (int)height() && (m_map->isWalkable(x,y)));
    }

public:

    DistanceMap()
    {
        m_stack.reserve(128*128);
    }

    DistanceMap(const StarDraftMap & map, size_t gx, size_t gy)
        : m_map(&map)
        , m_gx(gx)
        , m_gy(gy)
    {
        m_stack.reserve(128*128);
        compute(map, gx, gy);
    }

    void compute(const StarDraftMap & map, size_t gx, size_t gy)
    {
        m_gx  = gx;
        m_gy  = gy;
        m_map = &map;
    
        computeDistance();
    }
    
    int getDistance(size_t x, size_t y) const
    {
        return m_distance.get(x, y);
    }

    size_t getNumDirections(size_t x, size_t y) const
    {
        unsigned char dirs = m_directions.get(x, y);
        return dirs;
        size_t count = 0;
        while (dirs) {
            dirs &= (dirs - 1);
            count++;
        }
        return count;
    }

    Tile getDirection(size_t x, size_t y, size_t index) const
    {
        unsigned char dirs = m_directions.get(x, y);

        size_t count = 0;
        for (size_t bit = 0; bit < 8; bit++)
        {
            if (dirs & (1 << bit))
            {
                count++;
            }

            if (count == index + 1)
            {
                return m_actions[bit];
            }
        }
    
        return { 0,0 };
    }

    inline size_t width() const 
    { 
        return m_map ? m_map->width() : 0; 
    }

    inline size_t height() const 
    { 
        return m_map ? m_map->height() : 0; 
    }

    inline const std::vector<Tile> & getClosestTiles() const
    {
        return m_stack;
    }
};
