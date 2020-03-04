#pragma once

#include "GameState.h"
#include "Assets.h"

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class GameEngine
{

protected:

    std::vector<std::shared_ptr<GameState>> m_states;
    std::vector<std::shared_ptr<GameState>> m_statesToPush;
    sf::RenderWindow                        m_window;
    size_t                                  m_popStates = 0;
    bool                                    m_running = true;
    size_t                                  m_updates = 0;
	size_t									m_currentFrame = 0;

    void onFrame();

public:
    
    GameEngine();

    void init();

    void pushState(std::shared_ptr<GameState> state);
    void popState();

    void quit();
    void run();

    sf::RenderWindow & window();
    bool isRunning();
};