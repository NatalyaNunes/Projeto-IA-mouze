#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>
#include <iostream>
#include <string>

#include "direction.hpp"

class Level {
    public:
        int rows;
        int cols;
        std::vector<std::string> board;
        std::vector<Point> medium_dificulty;
        std::vector<Point> high_dificulty;
        int food_increment=0;
        Point start_mouse;
        Point current_mouse;
        Point food_mouse;
        
        Level() : rows(0), cols(0) {}

        void find_start_position();
        void generate_food();
        void reset_level(bool initial_level);
        void fill_data(Point head, bool dead);
       

        char get_cell(const Level& level, const Point& p);
        bool is_empty_cell(char cell);
        //bool is_snake_body(char cell);
        bool is_food(char cell);
        bool is_wall(char cell);
        
        const std::vector<std::string>& get_board() const;
        void update_board_after_food();
        Point get_mouse_start_position() const; 
        
};

#endif
