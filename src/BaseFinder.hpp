#pragma once

#include "Grid2D.hpp"
#include "StarDraftMap.hpp"
#include <algorithm>

#include <vector>

// a base is just a rectangle encompassing some portion of the map
// if translated back to BWAPI, just add all the resources in the rectabgle
struct ResourceBorder
{
    int left    = std::numeric_limits<int>::max();
    int right   = std::numeric_limits<int>::min();
    int top     = std::numeric_limits<int>::max();
    int bottom  = std::numeric_limits<int>::min();

    bool contains(int x, int y) const
    {
        return (x >= left) && (x <= right) && (y >= top) && (y <= bottom);
    }
};

struct ResourceCluster
{
    std::vector<Tile> minerals;
    std::vector<Tile> gas;
    std::vector<Tile> allResources;
};

class Base
{
    const StarDraftMap * m_map = nullptr;

    ResourceCluster m_cluster;
    ResourceBorder  m_border;

    DistanceMap     m_centerOfResourcesDistanceMap;
       
    bool            m_isStartLocation = false;
    Vec2            m_centerOfResources;

    Tile            m_depotTile;

public:

    Base(const StarDraftMap & map, const ResourceCluster & cluster, const ResourceBorder & border)
        : m_map(&map)
        , m_cluster (cluster)
        , m_border  (border)
    {
        // Step 1. Compute the center of resources
        for (auto & tile : m_cluster.allResources)
        {
            m_centerOfResources.x += tile.x;
            m_centerOfResources.y += tile.y;
        }

        m_centerOfResources = m_centerOfResources / m_cluster.allResources.size();
        Tile center = {(int)m_centerOfResources.x, (int)m_centerOfResources.y};

        // if the center of resources lands on a resource, move it by one
        if (!m_map->isWalkable(center.x, center.y))
        {
            Tile check[] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

            for (size_t i=0; i<8; i++)
            {
                Tile newTile = {check[i].x + center.x, check[i].y + center.y};
                if (m_map->isWalkable(newTile.x, newTile.y))
                {
                    center = newTile;
                    break;
                }
            }
        }

        m_centerOfResources.x = center.x;
        m_centerOfResources.y = center.y;

        // Step 2. Compute the distance map from the center of resources
        m_centerOfResourcesDistanceMap.compute(*m_map, (size_t)m_centerOfResources.x, (size_t)m_centerOfResources.y);

        // Step 3. Compute where the depot should go
        for (auto tile : m_centerOfResourcesDistanceMap.getClosestTiles())
        {
            if (m_map->canBuildBuildingOfSize(tile.x, tile.y, 4, 3, true))
            {
                m_depotTile = tile;
                break;
            }
        }
    }

    inline Vec2 getCenterOfResources() const
    {
        return m_centerOfResources;
    }

    inline Tile getDepotTile() const
    {
        return m_depotTile;
    }
};

class BaseFinder
{    
    struct Direction { char x = 0, y = 0; };

    const StarDraftMap *            m_map = nullptr;
    std::vector<ResourceBorder>     m_baseBorders;
    Grid2D<int>                     m_resourceDist;
    Grid2D<int>                     m_resourceClusterLabels;
    int                             m_maxDepth = 5;
    int                             m_ops = 0;
    std::vector<Base>               m_bases;
    std::vector<Tile>               m_stack;
    std::vector<Direction>          m_actions = { {0,1}, {0,-1}, {1,0}, {-1,0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    std::vector<ResourceCluster>    m_resourceClusters;
    
    void resourceDistanceBFS(int x, int y)
    {
        m_stack.clear();

        m_stack.push_back({x, y});
        m_resourceDist.set(x, y, 0);

        // iterate until the current pointer meets the end
        for (size_t i = 0; i < m_stack.size(); i++)
        {
            // get a copy of the cell and its distance to avoid future lookups
            Tile tile = m_stack[i];
            int dist = m_resourceDist.get(tile.x, tile.y);

            // if we've gone far enough away, stop
            if (dist == m_maxDepth) { return; }

            // look out in each direction
            for (size_t a = 0; a < m_actions.size(); a++)
            {
                // calculate the child cell
                Tile next ={tile.x + m_actions[a].x, tile.y + m_actions[a].y};

                // if this child is a valid cell we can do stuff with it
                if (m_map->isValid(next.x, next.y) && m_map->isWalkable(next.x, next.y))
                {
                    // save the distance to the next cell to avoid another lookup
                    auto nval = m_resourceDist.get(next.x, next.y);

                    // if we haven't seen this cell before
                    if (nval == -1)
                    {
                        // add it to the stack and set the distance
                        m_stack.emplace_back(next);
                        m_ops++;
                        m_resourceDist.set(next.x, next.y, dist + 1);
                    }
                }
            }
        }
    }

    // this will form a cluster of resource tiles by BFSing from x,y
    // x, y will be a resource tile of unassigned cluster
    ResourceCluster resourceClusterFormationBFS(int x, int y, int clusterNumber)
    {
        m_stack.clear();
        m_stack.push_back({x, y});

        // the vector of resource tiles for this cluster
        ResourceCluster cluster;

        // iterate until the current pointer meets the end
        for (size_t i = 0; i < m_stack.size(); i++)
        {
            // get a copy of the cell and its distance to avoid future lookups
            Tile tile = m_stack[i];

            if (m_map->isMineral(tile.x, tile.y)) { cluster.minerals.push_back(tile); }
            if (m_map->isGas(tile.x, tile.y)) { cluster.gas.push_back(tile); }
            if (m_map->isResource(tile.x, tile.y)) { cluster.allResources.push_back(tile); }

            // look out in each direction
            for (size_t a = 0; a < m_actions.size(); a++)
            {
                // calculate the child cell
                Tile next ={tile.x + m_actions[a].x, tile.y + m_actions[a].y};

                // test the child tile for validity
                if (m_map->isValid(next.x, next.y) &&               // if this child is a valid cell
                    m_resourceDist.get(next.x, next.y) != -1 &&     //    and it has a valid distance
                    m_resourceClusterLabels.get(next.x, next.y) == -1)    //    and it hasn't been given a cluster yet
                {
                    // mark this tile as being part of this cluster
                    m_resourceClusterLabels.set(next.x, next.y, clusterNumber);
                    m_stack.emplace_back(next);
                    m_ops++;
                }
            }
        }

        return cluster;
    }


public:

    BaseFinder()
    {
        m_stack.reserve(1024);
    }

    BaseFinder(const StarDraftMap & map)
        : m_map(&map)
    {
        m_stack.reserve(1024);

        computeBases(map);
    }

    void computeBases(const StarDraftMap & map)
    {
        m_map = &map;

        m_baseBorders ={};
        m_resourceDist = Grid2D<int>(m_map->width(), m_map->height(), -1);
        m_resourceClusterLabels = Grid2D<int>(m_map->width(), m_map->height(), -1);

        // Step 1. BFS outward from resources
        for (auto resourceTile : m_map->resourceTiles())
        {
            resourceDistanceBFS(resourceTile.x, resourceTile.y);
        }

        // Step 2. Label each resource cluster
        int clusterNumber = 0;
        for (auto resourceTile : m_map->resourceTiles())
        {
            // if this tile has already been clustered skip it 
            if (m_resourceClusterLabels.get(resourceTile.x, resourceTile.y) != -1) { continue; }

            // perform the BFS on this un-clustered resource tile to assign it a cluster number 
            m_resourceClusters.push_back(resourceClusterFormationBFS(resourceTile.x, resourceTile.y, clusterNumber++));
        }

        // Step 3. Form resource borders out of each cluster, if they are valid
        for (auto & cluster : m_resourceClusters)
        {
            // if there aren't enough resources in the cluster, it won't be a base
            if (cluster.minerals.size() < 4) { continue; }

            ResourceBorder border;

            for (auto tile : cluster.allResources)
            {
                border.left     = std::min(border.left, tile.x);
                border.right    = std::max(border.right, tile.x);
                border.top      = std::min(border.top, tile.y);
                border.bottom   = std::max(border.bottom, tile.y);
            }

            m_baseBorders.push_back(border);

            // Step 4. Form a base out of the cluster and resource border
            m_bases.push_back(Base(*m_map, cluster, border));
        }        
    }
    
    const Grid2D<int> & getResourceDist() const
    {
        return m_resourceDist;
    }

    const Grid2D<int> & getClusterLabels() const
    {
        return m_resourceClusterLabels;
    }

    const std::vector<ResourceBorder> & getBaseBorders() const
    {
        return m_baseBorders;
    }

    const std::vector<Base> & getBases() const
    {
        return m_bases;
    }
};