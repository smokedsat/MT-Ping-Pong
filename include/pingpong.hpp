#pragma once

#include <mutex>
#include <iostream>

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