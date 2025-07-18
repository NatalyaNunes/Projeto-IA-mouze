#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "level.hpp"

#include <map>

void help_screen(std::string_view msg="");
void render_board(const Level& level_to_draw);
void run_tests();

#endif