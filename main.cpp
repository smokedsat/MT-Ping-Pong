#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>

std::condition_variable condition_;
std::mutex mutex_;
bool flag = true;

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
    //bool & _queue;
    //PingPong & _pingpong;

    void resize(int count)
    {
        if(_gamers.size() > count)
        {
            while(_gamers.size() != count)
            {
                _gamers.pop_back();
            }
        }
        else if(_gamers.size() < count)
        {
            while(_gamers.size() != count)
            {
                _gamers.push_back(Gamer(_gamers.size(),_team_id));
            }
        }
    }

    Team(int team_id,int team_size) : 
    _team_id(team_id), _team_size(team_size) 
    { 
        for(auto i = 0; i < _team_size; i++)
        {
            _gamers.push_back(Gamer(i + 1,_team_id));
        }
    }
};

void play(Team & team, PingPong & pingpong, bool & game_over)
{   
    std::unique_lock<std::mutex> lock(mutex_);

    while(!game_over)
    {
        for(auto i = 0; i < team._gamers.size(); i++)
            {
              condition_.wait(lock,[]{return flag?true:false;});
              pingpong.print(team._gamers[i].get_id(),team._team_size, team._team_id, pingpong.getValue());
              flag = !flag;
              condition_.notify_one();
            }
    }
}

int main()
{
    int team_size{100}, team_n1{1}, team_n2{2};
    PingPong pingpong;
    bool game_over = false;

    Team team_1(team_n1, team_size);
    Team team_2(team_n2, team_size);    

    std::thread thr1([&team_1, &pingpong, &game_over]()
    {
        std::unique_lock<std::mutex> lock(mutex_);

        while(!game_over)
        {
            for(auto i = 0; i < team_1._gamers.size(); i++)
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                condition_.wait(lock,[]{return flag?true:false;});
                pingpong.print(team_1._gamers[i].get_id(),team_1._team_size, team_1._team_id, pingpong.getValue());
                flag = !flag;
                condition_.notify_one();
            }
        }
    });

    std::thread thr2([&team_2, &pingpong, &game_over]()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(!game_over)
        {
            for(auto i = 0; i < team_2._gamers.size(); i++)
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                condition_.wait(lock,[]{return !flag?true:false;});
                pingpong.print(team_2._gamers[i].get_id(),team_2._team_size, team_2._team_id, pingpong.getValue());
                flag = !flag;
                condition_.notify_one();
            }
        }
    });

    thr1.join();
    thr2.join();

    return 0;
}