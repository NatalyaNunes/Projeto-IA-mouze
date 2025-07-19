#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include "level.hpp"
#include "mouse.hpp"
#include "player.hpp"
#include "direction.hpp"

class MouzeSimulation
{
   enum GameState : std::uint8_t{
    START=0,
    WELCOME, 
    LOAD_LEVEL, //gera a comida
    THINKING, //escolhe o caminho que a cobra vai fazer
    RUNNING, //aonde se mexe
    EATING, //achou a comida
    CRASHED, //bateu na parede ou nela mesma
    LEVEL_UP, //conferir se comeu toda a comida
    LOST, //sem vidas
    WON, //niveis finalizados
    END
   };


   private:

    GameState game_state;
    int fps;
    size_t lives;
    size_t food;
    std::string player_type;
    std::string level_filename;
    std::string config_filename;
    std::unique_ptr<Player> player;
    bool have_path = false;
    std::vector<Point> path_execute;
    size_t idx_path = 1;//0 é onde a cabeça já tá
    bool search_food = true;

    int rows;
    int cols;
    
    std::vector<Level> levels; 
    size_t current_level_idx = 0;
    bool has_level = false;
    bool initial_level = true;
    
    bool has_food = false;
    bool has_wall = false;
    bool has_none=false;
    Level level; 

    //da cobra
    bool dead = false;
    Point head_mouse;
    
   public:
    static MouzeSimulation& instance();

    MouzeSimulation();
    void open_process_file();
    bool is_full_food();
    void reset_food();
    bool is_over() const;
    void trim(std::string& s);
    bool ends_with(const std::string& str, const std::string& suffix);
    void clear_actions();

    //output.cpp 
    void help_screen(std::string_view msg="");
    void render_board(const Level& level_to_draw);
    void run_tests();
    
    //funções principais
    void initialize(int argc, char* argv[]);
    void process_events();
    void update();
    void render();
    void parse_config(const std::string& filename);
};

#endif