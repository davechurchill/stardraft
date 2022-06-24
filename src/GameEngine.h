#pragma once

#include "Scene.h"
#include "Assets.h"

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class GameEngine
{

protected:

    std::vector<std::shared_ptr<Scene>> m_scenes;
    std::vector<std::shared_ptr<Scene>> m_scenesToPush;
    sf::RenderWindow    m_window;
    size_t              m_popScenes = 0;
    bool                m_running = true;
    size_t              m_updates = 0;
	size_t              m_currentFrame = 0;

    void onFrame();

public:
    
    GameEngine();

    void init();

    void pushScene(std::shared_ptr<Scene> scene);
    void popScene();

    void quit();
    void run();

    sf::RenderWindow & window();
    bool isRunning();
};
