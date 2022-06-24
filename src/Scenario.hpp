#pragma once

#include <iostream>
#include <fstream>
#include <vector>

struct Event
{
    std::string type;
    std::string unitType;
    int frame = 0;
    int id = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
};

class Scenario
{
    std::string m_path;
    std::vector<Event> m_events;
    std::string m_map;

public:

    Scenario() {}
    
    void load(const std::string& path)
    {
        m_path = path;
        std::ifstream fin(path);
        std::string token;
        m_events.clear();

        // first token of the file is the map path (no spaces)
        fin >> m_map;
        
        Event e;
        while (fin >> e.frame)
        {
            e = Event();
            fin >> e.type;
            if (e.type == "add")
            {
                fin >> e.unitType >> e.x1 >> e.y1 >> e.x2 >> e.y2;
            }
            else if (e.type == "del")
            {
                fin >> e.unitType >> e.x1 >> e.y1 >> e.x2 >> e.y2;
            }
            else if (e.type == "path")
            {
                fin >> e.unitType >> e.id >> e.x1 >> e.y1 >> e.x2 >> e.y2;
            }
            m_events.push_back(e);
        }
    }

    const std::vector<Event>& events() const
    {
        return m_events;
    }

    const std::string& getMapPath() const
    {
        return m_map;
    }

    const std::string& getPath() const
    {
        return m_path;
    }
};
