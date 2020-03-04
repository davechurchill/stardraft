#pragma once

#include "GameState.h"
#include "WorldView.hpp"
#include "StarcraftMap.hpp"
#include "VectorField.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <iostream>

class GameState_Map : public GameState
{   
    sf::Font m_font;             
    sf::Text m_text;

    int  m_tileSize         = 32;
    bool m_drawGrid         = true;
    bool m_drawWalkTiles    = true;
    bool m_drawBuildTiles   = true;
    bool m_drawField        = true;
    bool m_drawDistance     = true;
    bool m_leftMouseDown    = false;

    int m_pgx = -1;
    int m_pgy = -1;

    sf::VertexArray     m_buildTileArray;
    sf::VertexArray     m_walkTileArray;

    const std::string   m_mapFile;
    StarcraftMap        m_map;            
    VectorField         m_field;

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
