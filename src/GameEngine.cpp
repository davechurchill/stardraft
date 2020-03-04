#include "GameEngine.h"
#include "Assets.h"

#include "GameState_Menu.h"
#include "GameState_Map.h"

GameEngine::GameEngine()
{
    init();
}

void GameEngine::init()
{
    m_window.create(sf::VideoMode(1280, 720), "StarDraft");
    m_window.setFramerateLimit(60);

    Assets::Instance().addFont("Consolas", "fonts/Consolas.ttf");

    pushState(std::make_shared<GameState_Menu>(*this));
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

        if (m_states.empty())
        {
            quit();
        }
    }
}

void GameEngine::pushState(std::shared_ptr<GameState> state)
{
    m_statesToPush.push_back(state);
}

void GameEngine::popState()
{
    m_popStates++;
}

void GameEngine::onFrame()
{
    if (!isRunning()) { return; }
    m_updates++;
    
    // pop however many states off the state stack as we have requested
    for (size_t i = 0; i < m_popStates; i++)
    {
        if (!m_states.empty())
        {
            m_states.pop_back();
        }
    }
    // reset the state stack pop counter
    m_popStates = 0;

    // push any requested states onto the stack
    for (size_t i = 0; i < m_statesToPush.size(); i++)
    {
        m_states.push_back(m_statesToPush[i]);
    }
    m_statesToPush.clear();

    // call update on the top of the stack (current state)
    if (!m_states.empty())
    {
        m_states.back()->onFrame();
    }
}

void GameEngine::quit()
{
    m_running = false;
}
