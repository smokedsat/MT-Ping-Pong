#pragma once

#include <condition_variable>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <thread>

using json = nlohmann::json;

struct DataReader {
private:
  json json_data;
  std::ifstream file;
  std::string filename = "data.json";
  int prev_team_size = 0;
  bool prev_game_over = false;
  std::condition_variable cond;
  bool & _data_readed;
  bool & _data_changed;
  int & _team_size;
  bool & _game_over;
  std::condition_variable & _condition;

public:
  int curr_team_size = 0;
  bool curr_game_over = false;

  DataReader(): _data_changed(prev_game_over), _team_size(prev_team_size),
  _game_over(prev_game_over),_condition(cond), _data_readed(prev_game_over) {
    file.open(filename);

    if (file.is_open()) {
      json_data = json::parse(file);
      file.close();
    } else {
      std::cout << "File " << filename << " is not opened" << std::endl;
      std::cout << "Please make sure data.json is nearly to pragram file" << std::endl;
    }
  }

  DataReader(bool & data_readed, bool & data_changed,bool & game_over, int & team_size, std::condition_variable & condition) : 
    _data_changed(data_changed), _team_size(team_size), _game_over(game_over), _condition(condition), _data_readed(data_readed)
  {
    file.open(filename);

    if (!file.is_open())
    {
      filename = filename + ".txt";
      file.open(filename);
    }
    
    if (file.is_open()) {
      json_data = json::parse(file);
      _team_size = getTeamSizeFromJSON();
      _game_over = getGameStatusFromJSON();
      data_readed = true;
      file.close();
      
    } else {
      std::cout << "File " << filename << " is not opened" << std::endl;
    }
  }

  bool isDataReaded()
  {
    return _data_readed;
  }

  void printData()
  {
    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Game status: " << curr_game_over << std::endl;
    std::cout << "Team size: " << curr_team_size << std::endl;
  }

  void updateJSON() {
      file.open(filename);
      if (file.is_open()) 
      {
        json_data.clear();
        json_data = json::parse(file);
        updateData();
        //printData();
        file.close();
      } 
      else 
      {
        std::cout << "Can't open file " << filename << std::endl;
        return;
      }
  }

  void watch()
  {
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        updateJSON();
    }
  }

private:
  void updateData() {
    int new_team_size = getTeamSizeFromJSON();
    bool new_game_over = getGameStatusFromJSON();

    if (new_team_size != curr_team_size || new_game_over != curr_game_over) {
      _data_changed = true;
      prev_team_size = curr_team_size;
      curr_team_size = new_team_size;
      _team_size = curr_game_over;
      prev_game_over = curr_game_over;
      curr_game_over = new_game_over;
      _game_over = curr_game_over;
      _condition.notify_all();
    }
    else
    {
        _data_changed =false;
    }
  }

  void setDataFromJSON()
  {
    _team_size = getTeamSizeFromJSON();
    _game_over = getGameStatusFromJSON();
  }

  int getTeamSizeFromJSON() {
    if (!json_data.empty()) {
      return json_data["team_size"];
    } else {
      return -1;
    }
  }

  bool getGameStatusFromJSON() {
    if (!json_data.empty()) {
      return json_data["game_over"];
    } else {
      return false;
    }
  }
};
