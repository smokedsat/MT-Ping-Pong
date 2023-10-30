#pragma once

struct Gamer
{
private:
  int _gamer_id;
  int _team_id;
public:
  Gamer(int gamer_id, int team_id) : _gamer_id(gamer_id), _team_id(team_id){ }
  Gamer(): _gamer_id(0), _team_id(0){}

  int get_id()
  {
    return _gamer_id;
  }

  int get_team()
  {
    return _team_id;
  }
};