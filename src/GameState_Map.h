#pragma once

#include "GameState.h"
#include "WorldView.hpp"
#include "StarDraftMap.hpp"
#include "DistanceMap.hpp"
#include "BaseFinder.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <iostream>

class GameState_Map : public GameState
{   
    sf::Font m_font;             
    sf::Text m_text;

    int  m_tileSize         = 32;       // size of tiles (keep 32)
    bool m_drawGrid         = true;     // draw the grid lines
    bool m_drawWalkTiles    = true;     // draw the walk tiles
    bool m_drawBuildTiles   = true;     // draw the build tiles
    bool m_drawField        = true;     // draw the distance map values?
    bool m_drawDistance     = true;     // draw the distance map (t) directions (f)
    bool m_leftMouseDown    = false;    // is the left mouse button down
    int m_pgx               = -1;       // previous goal x
    int m_pgy               = -1;       // previous goal y

    sf::VertexArray     m_buildTileArray;
    sf::VertexArray     m_walkTileArray;

    const std::string   m_mapFile;
    StarDraftMap        m_map;            
    DistanceMap         m_field;
    BaseFinder          m_baseFinder;

    Vec2                m_drag = { -1, -1 };
    Vec2                m_mouseScreen;
    Vec2                m_mouseWorld;
    Vec2                m_mouseGrid;
    
    WorldView           m_view;
    
    void init();  
    
    void setMapVertexArray();
    void sUserInput();  
    void sRender();
    
public:

    GameState_Map(GameEngine& game, const std::string & mapFile);

    void onFrame();
};
