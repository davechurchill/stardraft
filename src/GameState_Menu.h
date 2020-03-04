#pragma once

#include "GameState.h"
#include <map>
#include <memory>
#include <deque>


class GameState_Menu : public GameState
{

protected:
    
    std::string                 m_title;
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_levelPaths;
    sf::Text                    m_menuText;
    size_t                      m_selectedMenuIndex = 0;
    
    void init(const std::string & menuConfig);
    void onFrame();
    void sUserInput();
    void sRender();

public:

    GameState_Menu(GameEngine & game);

};