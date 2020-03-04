#include "GameState_Menu.h"
#include "GameState_Map.h"
#include "Assets.h"
#include "GameEngine.h"
#include <sstream>
#include <filesystem>

GameState_Menu::GameState_Menu(GameEngine & game)
    : GameState(game)
{
    init("");
}

void GameState_Menu::init(const std::string & menuConfig)
{
    m_title = "StarDraft: A StarCraft Map Visualizer";

    size_t num = 1;
    for(auto& p: std::filesystem::directory_iterator("maps"))
    {
        std::stringstream ss;
        ss << ((num < 10) ? " " : "") << (num) << ") " << p.path().string();
        num += 1;

        m_menuStrings.push_back(ss.str());
        m_levelPaths.push_back(p.path().string());
    }
        
    m_menuText.setFont(Assets::Instance().getFont("Consolas"));
    m_menuText.setCharacterSize(16);
}

void GameState_Menu::onFrame()
{
    sUserInput();
    sRender();
}

void GameState_Menu::sUserInput()
{
    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
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
                    m_game.quit(); 
                    break; 
                }
                case sf::Keyboard::W: 
                case sf::Keyboard::Up:
                {
                    if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
                    else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
                    break;
                }
                case sf::Keyboard::S: 
                case sf::Keyboard::Down:
                { 
                    m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size(); 
                    break; 
                }
                case sf::Keyboard::D: 
                case sf::Keyboard::Enter:
                { 
                    m_game.pushState(std::make_shared<GameState_Map>(m_game, m_levelPaths[m_selectedMenuIndex]));
                    break; 
                }
                default: break;
            }
        }
    }
}

void GameState_Menu::sRender()
{
    // clear the window to a blue
    m_game.window().setView(m_game.window().getDefaultView());
    m_game.window().clear(sf::Color(0, 0, 0));

    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(32);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setPosition(sf::Vector2f(12, 5));
    m_game.window().draw(m_menuText);
    
    m_menuText.setCharacterSize(16);
    const int filesPerLine = 38;
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::Yellow : sf::Color(127, 127, 127));
        m_menuText.setPosition(sf::Vector2f(32.0f + (float)(i/filesPerLine)*450, 50.0f + (i%filesPerLine) * (float)m_menuText.getCharacterSize()+2));
        m_game.window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(20);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setString("up: w   down: s   run: d   back: esc");
    m_menuText.setPosition(sf::Vector2f(10, 690));
    m_game.window().draw(m_menuText);

    m_game.window().display();
}