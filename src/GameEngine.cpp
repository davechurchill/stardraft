#include "GameEngine.h"
#include "Assets.h"

#include "Scene_Menu.h"
#include "Scene_Map.h"

GameEngine::GameEngine()
{
    init();
}

void GameEngine::init()
{
    m_window.create(sf::VideoMode(1280, 720), "StarDraft");
    m_window.setFramerateLimit(60);

    Assets::Instance().addFont("Consolas", "fonts/Consolas.ttf");
}

bool GameEngine::isRunning()
{ 
    return m_running && m_window.isOpen();
}

sf::RenderWindow & GameEngine::window()
{
    return m_window;
}

void GameEngine::run()
{
    while (isRunning())
    {
        onFrame();

        if (m_scenes.empty())
        {
            quit();
        }
    }
}

void GameEngine::pushScene(std::shared_ptr<Scene> Scene)
{
    m_scenesToPush.push_back(Scene);
}

void GameEngine::popScene()
{
    m_popScenes++;
}

void GameEngine::onFrame()
{
    if (!isRunning()) { return; }
    m_updates++;
    
    // pop however many Scenes off the Scene stack as we have requested
    for (size_t i = 0; i < m_popScenes; i++)
    {
        if (!m_scenes.empty())
        {
            m_scenes.pop_back();
        }
    }
    // reset the Scene stack pop counter
    m_popScenes = 0;

    // push any requested Scenes onto the stack
    for (size_t i = 0; i < m_scenesToPush.size(); i++)
    {
        m_scenes.push_back(m_scenesToPush[i]);
    }
    m_scenesToPush.clear();

    // call update on the top of the stack (current Scene)
    if (!m_scenes.empty())
    {
        m_scenes.back()->onFrame();
    }
}

void GameEngine::quit()
{
    m_running = false;
}
