#include <mutex>
#include <condition_variable>
#include <thread>

#include "../include/datareader.hpp" 
#include "../include/team.hpp"       
#include "../include/pingpong.hpp"   

std::condition_variable condition_;
std::mutex mutex_;
bool flag = true;

int main()
{
    std::mutex mutex_for_reading;
    std::condition_variable condition_for_reading;

    int team_size{3}, team_n1{1}, team_n2{2};

    PingPong pingpong;

    bool data_readed = false;
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
            team_1_resized = team_1.resize_bool(team_size);
            team_2_resized = team_2.resize_bool(team_size);

            if(team_1_resized && team_2_resized)
            {
                break;
            }   
        }
    }
     
    std::thread reader([&data_reader](){
        data_reader.watch();
    });

    while(true)
    {
        std::unique_lock<std::mutex> reader_lock(mutex_for_reading);
        condition_for_reading.wait(reader_lock, [&data_readed]{return data_readed;});
        
        std::thread thr1([&team_1, &pingpong, &game_over, &data_changed, &data_reader, &team_2_resized]()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(!game_over)
            {
                for(auto i = 0; i < team_1._gamers.size(); i++)
                {
                    condition_.wait(lock,[]{return flag?true:false;});
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

            if(game_over)
            {
                return;
            }
        });

        std::thread thr2([&team_2, &pingpong, &game_over, &data_changed, &data_reader, &team_1_resized]()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while(!game_over)
            {
                for(auto i = 0; i < team_2._gamers.size(); i++)
                {
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
