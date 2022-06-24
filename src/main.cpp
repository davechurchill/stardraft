#include "GameEngine.h"
#include "Scene_Menu.h"
#include "Scene_Map.h"
#include <memory>

int main()
{
    GameEngine engine;

    auto scene = std::make_shared<Scene_Map>(engine);
    scene->loadScenario("scenarios/TestScenario.txt");
    engine.pushScene(scene);

    //engine.pushState(std::make_shared<Scene_Menu>(engine));

    engine.run();
}

