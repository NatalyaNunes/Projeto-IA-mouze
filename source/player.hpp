#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "level.hpp"
#include "mouse.hpp"
#include "direction.hpp"

#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <deque>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <tuple>
#include <cmath>

struct PathUnit{
    Point current_pos;
    std::vector<Dir> path;//cmainho de direções
};


class Player{
    public:
        Player(const Level& lvl) : level(lvl){}
        std::unique_ptr<Mouse> mouse;
        
        Mouse m_mouse;
        size_t score = 0;
        size_t lives; 

        std::vector<Point> path;
        void computed_path_bt(Point head_mouse);
        void computed_path_A(Point head_mouse);
        bool has_path() const;
        bool get_valid_path() const;
        Dir get_direction();
        Point computed_random(const Point& head_mouse);
        
        std::vector<Dir> path_direction;
        Point get_next_head_position(Point head_position, Dir next_direction) const;
        const std::vector<Point>& get_body() const;
        void increase_mouse_size();
        size_t get_mouse_size() const;

        std::vector<Point> get_path() const{
            return path;
        }

        int get_terrain_cost(Point p);
        
    private:
        const Level& level;
        Dir direction_head{Dir::N};
        bool path_valid = true;
        bool is_valid(const Point& p) const;
        
};

#endif