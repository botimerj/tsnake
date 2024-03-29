#include <game.h>

////////////////
// Coor Class //
////////////////

Coor::Coor(unsigned int xpos, unsigned int ypos,
           unsigned int xmax, unsigned int ymax,
           unsigned int xmin, unsigned int ymin){
    this->xmax = xmax; this->ymax = ymax;
    this->xmin = xmin; this->ymin = ymin;
    x = xpos; y = ypos;
}

void Coor::inc_x(){ x += 2; if (x > xmax) x = xmax; }
void Coor::dec_x(){ x -= 2; if (x < xmin) x = xmin; }
void Coor::inc_y(){ y += 1; if (y > ymax) y = ymax; }
void Coor::dec_y(){ y -= 1; if (y < ymin) y = ymin; }

////////////////////
// Update Classes //
////////////////////

Uelement::Uelement(int x, int y, std::string str){
    this->x = x; this->y = y; this->str = str;
}

Uvector::Uvector(){
    full = "\u2588\u2588";
    shaded = "\u2593\u2593";
    blank = "  ";
}

void Uvector::push_back(Uelement e){
    do { } while (atomic_flag_test_and_set(&aflag));
    vec.push_back(e);
    atomic_flag_clear(&aflag);
}

std::string Uvector::to_string(){
    do { } while (atomic_flag_test_and_set(&aflag));
    std::string tmp;
    std::string pstring;
    for(int i = 0; i < vec.size(); i++){
        tmp = "\033["+std::to_string(vec[i].y)+";"+
                      std::to_string(vec[i].x)+"H"+
                      vec[i].str;
        pstring.append(tmp);
    }
    vec.clear();
    atomic_flag_clear(&aflag);
    return pstring;
}

////////////////
// Game Class //
////////////////

// Management functions
Game::Game(unsigned int WIDTH, unsigned int HEIGHT){

    srand(time(NULL));
    
    if(WIDTH % 2 == 0){ this->WIDTH = WIDTH; }
    else              { this->WIDTH = WIDTH - 1; }
    this->HEIGHT = HEIGHT;
    
    xmax = this->WIDTH-2;  xmin = 2;
    ymax = this->HEIGHT-1; ymin = 3;

    snake.push_back(Coor(0, 0, xmax, ymax, xmin, ymin));
    food = new Coor(1, 1, xmax, ymax, xmin, ymin);
    reset();

    running = true;
    t_game = std::thread(&Game::loop, this); 

    colors.push_back("\033[37m"); // white
    colors.push_back("\033[31m"); // red
    colors.push_back("\033[32m"); // green
    colors.push_back("\033[33m"); // brown
    colors.push_back("\033[34m"); // blue
    colors.push_back("\033[35m"); // magenta
    colors.push_back("\033[36m"); // cyan
    r = 252; g = 0; b = 0;
}

Game::~Game(){
    delete food;
    t_game.join();
    //delete head;
}

void Game::input_handler(char c){
    if (c == 'w' && heading != 2) heading_next=0;
    if (c == 'a' && heading != 1) heading_next=3;
    if (c == 's' && heading != 0) heading_next=2;
    if (c == 'd' && heading != 3) heading_next=1;
    
    if (c == 'm') update_speed(true);
    if (c == 'n') update_speed(false); 

    if (c == 'k') update_length(true);
    if (c == 'j') update_length(false); 

    if (c == ' ') spawn_food(); 
}

std::string Game::render(){
    return uvector.to_string();
}

void Game::border(){

    std::string title = "[Esc] ==tsnake==";
    uvector.push_back(Uelement(1, 1, title));

    std::string tb;
    for(int i = 0; i < WIDTH-1; i++){ tb.append("\u2550");}
    uvector.push_back(Uelement(1, 2,      tb));
    uvector.push_back(Uelement(1, HEIGHT, tb));

    std::string lr("\u2551"); 
    for(int i = 3; i < HEIGHT; i++){ 
        uvector.push_back(Uelement(WIDTH, i, lr.c_str()));
        uvector.push_back(Uelement(1,     i, lr.c_str()));
    }

    uvector.push_back(Uelement(1,     2,      std::string("\u2554")));
    uvector.push_back(Uelement(WIDTH, 2,      std::string("\u2557")));
    uvector.push_back(Uelement(1,     HEIGHT, std::string("\u255A")));
    uvector.push_back(Uelement(WIDTH, HEIGHT, std::string("\u255D")));
}

// Gameplay functions
void Game::update_speed(bool up){
    if(up){
        speed += 1;
    }
    else{
        speed -= 1;
        if (speed < 1) speed = 1;
    }

    char buf[16]; sprintf(buf, "%2d", speed);
    std::string tmp = "Speed: " + std::string(buf);
    uvector.push_back(Uelement(WIDTH-25, 1, tmp));
}

void Game::update_length(bool up){
    std::string tmp;
    if(up){
        snake.push_back(Coor(tail->x, tail->y, xmax, ymax, xmin, ymin));
        head = &snake[0];
        tail = &snake.back();
        length = snake.size();
    }
    else{
        if(length > 1){
            uvector.push_back(Uelement(tail->x, tail->y, std::string("  ")));
            snake.pop_back();
            head = &snake[0];
            tail = &snake.back();
            length = snake.size();
        }
    }

    char buf[16]; sprintf(buf, "%3d", length);
    tmp = "Length:" + std::string(buf);
    uvector.push_back(Uelement(WIDTH-38, 1, tmp));
}

void Game::update_clock(){
    char buf[16]; sprintf(buf, "%3.0f", gtime);
    std::string tmp = "Time(s):" + std::string(buf);
    uvector.push_back(Uelement(WIDTH-13, 1, tmp));
}

void Game::update_movement(){

    // Re-print food
    uvector.push_back(Uelement(food->x, food->y, uvector.full));

    heading = heading_next;
    uvector.push_back(Uelement(tail->x, tail->y, uvector.blank));

    if(r == 252 && b == 0){ g += 4;}
    if(g == 252 && b == 0){ r -= 4;}
    if(g == 252 && r == 0){ b += 4;}
    if(b == 252 && r == 0){ g -= 4;}
    if(b == 252 && g == 0){ r += 4;}
    if(r == 252 && g == 0){ b -= 4;}
    std::string rgb = "\033[38;2;"+std::to_string(r)+";"+\
                                   std::to_string(g)+";"+\
                                   std::to_string(b)+"m";
    uvector.push_back(Uelement(1, 1, rgb));

    for(std::vector<Coor>::iterator e = snake.end()-1; e != snake.begin(); --e){
        if(e != snake.begin()){
            e->x = (e-1)->x;
            e->y = (e-1)->y;
            uvector.push_back(Uelement(e->x, e->y, uvector.full));
        }
    }

    if      (heading == 0){ head->dec_y();
    }else if(heading == 1){ head->inc_x();
    }else if(heading == 2){ head->inc_y();
    }else if(heading == 3){ head->dec_x(); }

    uvector.push_back(Uelement(head->x, head->y, colors[0]));
    uvector.push_back(Uelement(head->x, head->y, uvector.full));

    // Detect losing condition
    if( coor_in_snake(*head) ){
        reset();
    }

    // Detect food
    if(head->x == food->x && head->y == food->y){
        spawn_food();
        update_length(true);
    }

    /*// Print coors
    std::string print_coors = "("+std::to_string(head->x)+","+
                                  std::to_string(head->y)+")";
    uvector.push_back(Uelement(1, HEIGHT, print_coors));*/

}

void Game::spawn_food(){
    // Create a new food (but make sure its not inside the snake)
    do{
        delete food;
        int food_x = (((rand()%(xmax-xmin)) + xmin)/2) * 2;
        //if(food_x % 2) food_x -= 1;
        int food_y = (rand()%(ymax-ymin)) + ymin;
        food = new Coor(food_x, food_y, xmax, ymax, xmin, ymin);
    }while(coor_in_snake(*food));
    uvector.push_back(Uelement(food->x, food->y, uvector.full));

    //std::string print_coors = "("+std::to_string(food->x)+","+
    //                              std::to_string(food->y)+")";
    //uvector.push_back(Uelement(1, HEIGHT, print_coors));
}

// State functions
void Game::loop(){
    //unsigned int tclk = 0;
    //unsigned int tmovement = 0;
    
    double twall     = get_wall_time();
    double twall_new = twall;
    double dt = 0;
    double tclk = 0;
    double tmovement = 0;

    while(running){
        if(tclk > 1){
            update_clock();
            tclk = 0;
        }
        //if(tmovement > 1000/speed){
        if(tmovement > 1.0/speed){
            update_movement();
            tmovement = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        twall_new = get_wall_time();
        dt = twall_new - twall;
        twall = twall_new;

        tclk += dt; tmovement += dt; gtime += dt;
        //tclk += 5; tmovement += 5; gtime += 5;

    }
}

void Game::reset(){
    std::string reset_str = "\033[H\033[2J";
    uvector.push_back(Uelement(1,1,reset_str));

    heading = 1;
    speed = 15;
    length = 1;
    
    snake.clear();

    snake.push_back(Coor((WIDTH/4)*2, HEIGHT/2, xmax, ymax, xmin, ymin));
    head = &snake[0]; tail = head;

    gtime = 0;
    border();
    update_speed(true);
    update_length(false);
    update_clock();

    spawn_food();
}

void Game::end(){
    running = false;
}

// Helper functions
bool Game::coor_in_snake(Coor test){
    // Test whether 'test' is contained in snake
    for(int i = 1; i < snake.size(); i++){
        if(snake[i].x == test.x && snake[i].y == test.y) return true;
    }
    return false;
}

double Game::get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)) return 0;
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

