#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>
#include <nlohmann/json.hpp>
#include <fstream>



using json = nlohmann::json;

struct PingPong
{
    PingPong() : value{"ping", "pong"}{}

    void hit_ball()
    {
        blocker.lock();
        mark = !mark;
        team_queue = !team_queue;
        blocker.unlock();
    }

    std::string getValue()
    {
        if(mark == true)
        {
        hit_ball();
        return value[1];
        }
        else
        {
        hit_ball();
        return value[0];
        }
    }

    bool & getQueue() { return team_queue; }
 
    void print(int player_id, int team_size, int team_id, const std::string & what)
    {
        blocker.lock();
        std::cout << player_id << "/" << team_size << "," << team_id << ":" << what << std::endl;
        blocker.unlock();
    } 
private:
  const std::string value[2];
  bool mark = false;
  bool team_queue = false;
  std::mutex blocker;
};


struct DataReader {
private:
  json json_data;
  std::ifstream file;
  const std::string filename = "data.json";
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
    }
  }

  DataReader(bool & data_readed, bool & data_changed,bool & game_over, int & team_size, std::condition_variable & condition) : 
    _data_changed(data_changed), _team_size(team_size), _game_over(game_over), _condition(condition), _data_readed(data_readed)
  {
    file.open(filename);

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
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    std::cout << "Game status: " << curr_game_over << std::endl;
    std::cout << "Team size: " << curr_team_size << std::endl;
  }

  void updateJSON() {
    while (true) {
      file.open(filename);
      if (file.is_open()) {
        json_data.clear();
        json_data = json::parse(file);
        updateData();
        printData();
        file.close();
      } else {
        std::cout << "Can't open file " << filename << std::endl;
        return;
      }
    }
  }

  void watch()
  {
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        updateJSON();
        _condition.notify_all();
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

struct Status
{
    bool game_over = false;
    bool & get_status() { return game_over;}
};

struct Team
{
    std::deque<Gamer> _gamers;
    int _team_id;
    int _team_size;
    bool isResized = false;
    //bool & _queue;
    //PingPong & _pingpong;

    bool resize_bool(int count)
    {
        std::cout << "Changing size of team: " << _team_id << " to " << count << std::endl;
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
            while(_gamers.size() != count)
            {
                _gamers.push_back(Gamer(_gamers.size(),_team_id));
            }
            _team_size = _gamers.size();
        }
        std::cout << "Done changing. Current size of team " << _team_id << " :" << _team_size << std::endl;
        return true;
    }

    void resize_void(int count) 
    {
        std::cout << "Changing size of team: " << _team_id << " to " << count << std::endl;
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
            while(_gamers.size() != count)
            {
                _gamers.push_back(Gamer(_gamers.size(),_team_id));
            }
            _team_size = _gamers.size();
        }
        std::cout << "Done changing. Current size of team " << _team_id << " :" << _team_size << std::endl;
    }

    Team(int team_id,int team_size) : 
    _team_id(team_id), _team_size(team_size)//, _data_changed(data_changed)
    { 
        for(auto i = 0; i < _team_size; i++)
        {
            _gamers.push_back(Gamer(i + 1,_team_id));
        }
    }
};

void resizeTeams(Team & team_1, Team & team_2, int size)
{
    team_1.resize_void(size);
    team_2.resize_void(size);
}

std::condition_variable condition_;
std::mutex mutex_;
bool flag = true;

int main()
{
    std::mutex mutex_for_reading;
    std::condition_variable condition_for_reading;
    bool data_readed = false;

    int team_size{3}, team_n1{1}, team_n2{2};
    PingPong pingpong;
    bool game_over = false;
    bool data_changed = false;
    
    bool team_1_resized = false;
    bool team_2_resized = false;

    DataReader data_reader(data_readed, data_changed, game_over, team_size, condition_for_reading);
    
    Team team_1(team_n1, team_size);
    Team team_2(team_n2, team_size);   

    while(true)
    {
        if(data_reader.isDataReaded())
        {
            team_1.resize_void(team_size);
            team_2.resize_void(team_size);
            break;
        }
    }
     
    std::thread reader([&data_reader](){
        data_reader.watch();
    });

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::unique_lock<std::mutex> reader_lock(mutex_for_reading);
        condition_for_reading.wait(reader_lock, [&data_readed]{return data_readed;});
        
        std::thread thr1([&team_1, &pingpong, &game_over, &data_changed, &data_reader, &team_2_resized]()
        {
            std::cout << "Created new thread 1" << std::endl;
            std::unique_lock<std::mutex> lock(mutex_);
            
            while(!game_over)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cout << "entered while!game_over 1" << std::endl;
                for(auto i = 0; i < team_1._gamers.size(); i++)
                {
                    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    condition_.wait(lock,[]{return flag?true:false;});
                    /*if(data_changed)
                    {
                        team_1.resize(data_reader.curr_team_size);
                        break;
                    }*/
                    pingpong.print(team_1._gamers[i].get_id(),team_1._team_size, team_1._team_id, pingpong.getValue());
                    flag = !flag;
                    condition_.notify_one();
                }

                if(data_changed)
                {
                    condition_.wait(lock,[&team_1, &data_reader]{return team_1.resize_bool(data_reader.curr_team_size);});
                    condition_.notify_one();
                }
            }
            condition_.notify_one();
            lock.unlock();
            std::cout << "Team_1 ends." << std::endl;

            if(game_over)
            {
                return;
            }
        });

        std::thread thr2([&team_2, &pingpong, &game_over, &data_changed, &data_reader, &team_1_resized]()
        {
            std::cout << "Created new thread 2" << std::endl;

            std::unique_lock<std::mutex> lock(mutex_);
            while(!game_over)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cout << "entered while!game_over 2" << std::endl;
                for(auto i = 0; i < team_2._gamers.size(); i++)
                {
                    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    condition_.wait(lock,[]{return !flag?true:false;});
                    pingpong.print(team_2._gamers[i].get_id(),team_2._team_size, team_2._team_id, pingpong.getValue());
                    flag = !flag;
                    condition_.notify_one();
                }
                if(data_changed)
                {
                    condition_.wait(lock,[&team_2, &data_reader]{return team_2.resize_bool(data_reader.curr_team_size);});
                    condition_.notify_one();
                }
            }
            condition_.notify_one();
            lock.unlock();
            std::cout << "Team_2 ends." << std::endl;

            if(game_over)
            {
                return;
            }
        });

        thr1.join();
        thr2.join();
        
        if(game_over)
        {
            break;
        }
    }
    
    return 0;
}
