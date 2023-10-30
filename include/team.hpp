#pragma once

#include <deque>
#include "gamer.hpp"

struct Team
{
    std::deque<Gamer> _gamers;
    int _team_id;
    int _team_size;
    bool isResized = false;

    bool resize_bool(int count)
    {
        if(count < 0)
        {
            count = 0;
        }
        if(_gamers.size() > count)
        {
            while(_gamers.size() != count)
            {
                _gamers.pop_back();
            }
            _team_size = _gamers.size();
        }
        else if(_gamers.size() < count)
        {
            int startingId = _gamers.empty() ? 1 : _gamers.back().get_id() + 1;
            while(_gamers.size() != count)
            {
                _gamers.push_back(Gamer(startingId, _team_id));
                startingId++;
            }
            _team_size = _gamers.size();
        }
        return true;
    }

    Team(int team_id,int team_size) : 
    _team_id(team_id), _team_size(team_size)
    { 
        if(team_size < 0)
        {
            _team_size = 0;
        }
        else if(team_size == 0)
        {
            _team_size = 0;
        }

        for(auto i = 1; i < _team_size; i++)
        {
            _gamers.push_back(Gamer(i,_team_id));
        }
    }
};
