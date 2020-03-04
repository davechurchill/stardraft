#include "GameState.h"
#include "GameEngine.h"

GameState::GameState(GameEngine & game)
    : m_game(game)
    , m_lineStrip(sf::LinesStrip)
{ 
    
}

void GameState::setPaused(bool paused)
{
    m_paused = paused;
}

const GameEngine& GameState::getEngine() const
{
	return m_game;
}

void GameState::playSound(const std::string& soundName)
{
    Assets::Instance().getSound(soundName).play();
}

size_t GameState::currentFrame() const
{
    return m_currentFrame;
}

void GameState::drawLine(double x1, double y1, double x2, double y2, sf::Color color)
{
    sf::Vertex v1(sf::Vector2f((float)x1, (float)y1), color);
    sf::Vertex v2(sf::Vector2f((float)x2, (float)y2), color);

    if (m_lineStrip.getVertexCount() > 0)
    {
        m_lineStrip.append(sf::Vertex(v1.position, sf::Color(0, 0, 0, 0)));
    }

    m_lineStrip.append(v1);
    m_lineStrip.append(v2);
    m_lineStrip.append(sf::Vertex(v2.position, sf::Color(0, 0, 0, 0)));
}

void GameState::drawLine(int x1, int y1, int x2, int y2, sf::Color color)
{
    drawLine((double)x1, (double)y1, (double)x2, (double)y2, color);
}