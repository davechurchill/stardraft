#include "GameState_Map.h"
#include "GameEngine.h"
#include "WorldView.hpp"
#include "StarcraftMap.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

GameState_Map::GameState_Map(GameEngine& game, const std::string & mapFile)
    : GameState(game)
    , m_mapFile(mapFile)
{
    m_map = StarcraftMap(mapFile);

    m_view.setWindowSize(Vec2(m_game.window().getSize().x, m_game.window().getSize().y));

    m_view.setView(m_game.window().getView());
    m_view.zoomTo(6, { 0, 0 });
    m_view.move({ -m_tileSize*3, -m_tileSize*3 });
        
    m_font.loadFromFile("fonts/Consolas.ttf");
    m_text.setFont(m_font);
    m_text.setPosition(10, 5);
    m_text.setCharacterSize(10);

    setMapVertexArray();
}

void GameState_Map::init()
{

}

void GameState_Map::onFrame()
{
    m_view.update();
    sUserInput();
    sRender(); 
    m_currentFrame++;
}

void GameState_Map::sUserInput()
{
    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_game.quit();
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                {
                    m_game.popState();
                    break;
                }
                case sf::Keyboard::E: break;
                case sf::Keyboard::W: m_view.zoom(0.8); break;
                case sf::Keyboard::A: break;
                case sf::Keyboard::S: break;
                case sf::Keyboard::D: m_view.zoom(1.2); break;
                case sf::Keyboard::G: m_drawGrid = !m_drawGrid; break;
                case sf::Keyboard::B: m_drawBuildTiles = !m_drawBuildTiles; break;
                case sf::Keyboard::F: m_drawField = !m_drawField; break;
                case sf::Keyboard::V: m_drawDistance = !m_drawDistance; break;
                case sf::Keyboard::T: m_drawWalkTiles = !m_drawWalkTiles; break;
                default: break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            // happens when the left mouse button is pressed
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                m_view.stopScroll();

                Vec2 worldPos = m_view.windowToWorld({event.mouseButton.x, event.mouseButton.y});
                
                int gridX = ((int)worldPos.x)/m_tileSize;
                int gridY = ((int)worldPos.y)/m_tileSize;

                if (gridX != m_pgx || gridY != m_pgy)
                {
                    m_field.compute(&m_map, gridX, gridY);
                    m_pgx = gridX; 
                    m_pgy = gridY;
                }

                m_leftMouseDown = true;
            }

            // happens when the right mouse button is pressed
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                m_drag = { event.mouseButton.x, event.mouseButton.y };
                m_view.stopScroll();
            }
        }

        // happens when the mouse button is released
        if (event.type == sf::Event::MouseButtonReleased)
        {
            // let go of the currently selected rectangle
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                m_leftMouseDown = false;
            }

            // let go of the currently selected rectangle
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                m_drag = { -1, -1 };
            }
        }

        if (event.type == sf::Event::MouseWheelMoved)
        {
            double zoom = 1.0 - (0.2 * event.mouseWheel.delta);
            m_view.zoomTo(zoom, Vec2(event.mouseWheel.x, event.mouseWheel.y));
        }

        // happens whenever the mouse is being moved
        if (event.type == sf::Event::MouseMoved)
        {
            // dragging with rmb
            if (m_drag.x != -1)
            {
                auto prev = m_view.windowToWorld(m_drag);
                auto curr = m_view.windowToWorld({ event.mouseMove.x, event.mouseMove.y });
                auto scroll = prev - curr;
                m_view.scroll(prev - curr);
                m_drag = { event.mouseMove.x, event.mouseMove.y };
            }

            // computing with lmb
            if (m_leftMouseDown)
            {
                Vec2 worldPos = m_view.windowToWorld({event.mouseMove.x, event.mouseMove.y});
                
                int gridX = ((int)worldPos.x)/m_tileSize;
                int gridY = ((int)worldPos.y)/m_tileSize;

                if (gridX != m_pgx || gridY != m_pgy)
                {
                    m_field.compute(&m_map, gridX, gridY);
                    m_pgx = gridX; 
                    m_pgy = gridY;
                }
            }
        }
    }
}

void GameState_Map::setMapVertexArray()
{
    m_buildTileArray = sf::VertexArray(sf::Quads);
    m_walkTileArray = sf::VertexArray(sf::Quads);
    
    std::map<char, sf::Color> colors =
    {
        { 'B', sf::Color(127, 127, 127) },
        { 'W', sf::Color(200, 200, 20) },
        { 'U', sf::Color(0, 0, 0) },    
        { 'D', sf::Color(90, 90, 90) },
        { 'M', sf::Color(0, 220, 220) },
        { 'G', sf::Color(20, 200, 20) }
    };

    // draw the map tiles
    for (size_t x = 0; x < m_map.width(); x++)
    {
        for (size_t y = 0; y < m_map.height(); y++)
        {
            float left   = (float)x * m_tileSize;
            float top    = (float)y * m_tileSize;
            float right  = left + m_tileSize;
            float bottom = top  + m_tileSize;

            sf::Color & color = colors[m_map.get(x, y)];
            m_buildTileArray.append(sf::Vertex({left,   top},    color));
            m_buildTileArray.append(sf::Vertex({right,  top},    color));
            m_buildTileArray.append(sf::Vertex({right,  bottom}, color));
            m_buildTileArray.append(sf::Vertex({left,   bottom}, color));
            
        }
    }

    for (size_t x = 0; x < 4*m_map.width(); x++)
    {
        for (size_t y = 0; y < 4*m_map.height(); y++)
        {
            float left   = (float)x * m_tileSize/4;
            float top    = (float)y * m_tileSize/4;
            float right  = left     + m_tileSize/4;
            float bottom = top      + m_tileSize/4;

            sf::Color color = (m_map.getWalk(x,y) == '1') ? sf::Color(127, 127, 127, 100) : sf::Color(0, 0, 0 , 100);
            m_walkTileArray.append(sf::Vertex({left,   top},    color));
            m_walkTileArray.append(sf::Vertex({right,  top},    color));
            m_walkTileArray.append(sf::Vertex({right,  bottom}, color));
            m_walkTileArray.append(sf::Vertex({left,   bottom}, color));
        }
    }

    sf::Color startColor = sf::Color::Red;
    for (auto & startTile : m_map.startTiles())
    {
        for (size_t x=0; x < 4; x++)
        {
            for (size_t y=0; y < 3; y++)
            {
                float left   = ((float)x + startTile.x) * m_tileSize;
                float top    = ((float)y + startTile.y) * m_tileSize;
                float right  = left + m_tileSize;
                float bottom = top + m_tileSize;

                m_buildTileArray.append(sf::Vertex({left,   top},    startColor));
                m_buildTileArray.append(sf::Vertex({right,  top},    startColor));
                m_buildTileArray.append(sf::Vertex({right,  bottom}, startColor));
                m_buildTileArray.append(sf::Vertex({left,   bottom}, startColor));
            }
        }
    }


}

// renders the scene
void GameState_Map::sRender()
{
    m_game.window().clear();
    m_lineStrip.clear();
    static float gs = (float)m_tileSize;
    m_game.window().setView(m_view.getSFMLView());
        
    sf::RectangleShape tile;
    tile.setFillColor(sf::Color(40, 40, 40));
    tile.setSize(sf::Vector2f((float)m_tileSize, (float)m_tileSize));

    // draw the vertical grid lines
    static sf::Color gridColor(16, 16, 16);
    
    // draw the build tiles
    if (m_drawBuildTiles)
    {
        m_game.window().draw(m_buildTileArray);
    }

    if (m_drawWalkTiles)
    {
        m_game.window().draw(m_walkTileArray);
    }

    // draw grid lines
    for (int x = 0; m_drawGrid && x <= (int)m_map.width(); x++)
    {
        drawLine(x*m_tileSize, 0, x*m_tileSize, m_map.height() * m_tileSize, gridColor);
    }

    for (int y = 0; m_drawGrid && y <= (int)m_map.height(); y++)
    {
        drawLine(0, y*m_tileSize, m_map.width() * m_tileSize, y*m_tileSize, gridColor);
    }

    int textSize = 24;
    static sf::RectangleShape rect;
    rect.setFillColor(sf::Color(16, 16, 16, 220));
    rect.setSize(sf::Vector2f(textSize * 20.0f, textSize * 8.0f));
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(4);
    rect.setPosition((float)(m_game.window().getSize().x- rect.getSize().x - 32), 16);

    // draw vector field values
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::Black);
    if (m_drawField && m_field.width() > 0)
    {
        for (size_t x=0; x < m_map.width(); x++)
        {
            for (size_t y=0; y < m_map.height(); y++)
            {
                if (m_drawDistance)
                {
                    int dist = m_field.getDistance(x, y);
                    m_text.setString(std::to_string(dist));
                    m_text.setPosition({(float)x*m_tileSize + 8, (float)y*m_tileSize + 8});
                    m_game.window().draw(m_text);
                }
                else
                {
                    // for each of the directions in the direction vector field
                    for (size_t d=0; d < m_field.getNumDirections(x,y); d++)
                    {
                        auto dir = m_field.getDirection(x, y, d);

                        // determine the start and end points of the direction vector
                        int cx = (int)(x * m_tileSize + m_tileSize / 2);
                        int cy = (int)(y * m_tileSize + m_tileSize / 2);
                        int dx = (int)(cx + (m_tileSize / 3.0f * dir.x));
                        int dy = (int)(cy + (m_tileSize / 3.0f * dir.y));

                        // draw the direction vector
                        drawLine(cx, cy, dx, dy, sf::Color(0, 0, 0));
                    }
                }
            }
        }
    }
    
    m_game.window().draw(m_lineStrip);

    m_game.window().setView(m_game.window().getDefaultView());

    std::stringstream ss;
    ss << m_mapFile << "\n";
    ss << "Size:   (" << m_map.width() << ", " << m_map.height() << ") Build Tiles\n";
    ss << "        (" << 4*m_map.width() << ", " << 4*m_map.height() << ") Walk Tiles\n";
    ss << "Memory: "  << (17*m_map.width()*m_map.height())/1000 << "kb\n";
    ss << "Players: " << m_map.startTiles().size();
    
    m_text.setFillColor(sf::Color::White);
    m_text.setString(ss.str());
    m_text.setPosition(rect.getPosition() + sf::Vector2f(15, 10));
    m_text.setCharacterSize(textSize);
    
    m_game.window().draw(rect);
    m_game.window().draw(m_text);
    // call the screen to display(), which swaps the buffers
    m_game.window().display();
}