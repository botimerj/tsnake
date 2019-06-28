#ifndef GAME_H
#define GAME_H

#include <atomic>
#include <thread>
#include <string>
#include <time.h>
#include <vector>
#include <sys/time.h>


class Coor{
public:
    unsigned int x;
    unsigned int y;

    unsigned int xmax, xmin;
    unsigned int ymax, ymin;

    Coor(unsigned int xpos, unsigned int ypos,
         unsigned int xmax, unsigned int ymax,
         unsigned int xmin, unsigned int ymin);
    void inc_x();
    void inc_y();
    void dec_x();
    void dec_y();
};

class Uelement{
public:
    std::string str;
    int x, y;
    Uelement(int x, int y, std::string str);
};

class Uvector{
public:
    std::atomic_flag aflag = ATOMIC_FLAG_INIT;
    std::vector <Uelement> vec;

    Uvector();

    std::string full;
    std::string shaded;
    std::string blank;

    void push_back(Uelement e);
    std::string to_string();
    //void clear();
};

class Game{
public:

    // Game parameters 
    std::thread t_game;
    unsigned int WIDTH;
    unsigned int HEIGHT;
    int xmax, ymax, xmin, ymin;
    bool running;

    // Game variables
    double gtime;
    unsigned int speed;
    unsigned int length;
    unsigned int heading;
    unsigned int heading_next;
    Coor * head;
    Coor * tail;
    Coor * food;
    std::vector<Coor> snake;

    std::vector<std::string> colors;
    int cidx;
    int r,g,b;

    // Screen coords to update
    Uvector uvector;

    // Management functions 
    Game(unsigned int w, unsigned int h);
    ~Game();
    void input_handler(char c);
    std::string render();
    void border();

    // State functions
    void loop();
    void reset();
    void end();

    // Gameplay functions 
    void update_speed(bool up);
    void update_length(bool up);
    void update_clock();
    void update_movement();
    void spawn_food();

    // Helper functions
    double get_wall_time();
    bool   coor_in_snake(Coor test_coor);
};

#endif

