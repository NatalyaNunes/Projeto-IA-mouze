#include <iostream>
#include <string>

#include "output.hpp"
#include "simulation.hpp"

void MouzeSimulation::help_screen(std::string_view msg){
    if(!msg.empty()){
        std::cout<<"Error: "<<msg<<"\n\n"; //mensagem de erro que será chamada na validação dos argumentos
    }
    std::cout << "Usage: mouze [<options>] <input_level_file>\n\n";
    std::cout << "Game simulation options:\n";
    std::cout << "  --help           Print this help text.\n";
    std::cout << "  --fps <num>      Number of frames (board) presented per second.\n";
    std::cout << "  --lives <num>    Number of lives the snake shall have. Default = 5.\n";
    std::cout << "  --food <num>     Number of food pellets for the entire simulation. Default = 10.\n";
    std::cout << "  --playertype <type> Type of snake intelligence: random, backtracking ou A*. Default = A*.\n";
}

void MouzeSimulation::render_board(const Level& level_to_draw) {   
    std::vector<std::string> temp_board = level_to_draw.board; // cópia local

    std::cout << "---------------------- MouzeAi -----------------------\n";
    std::cout << "Lives: ";
    for(size_t i = 0; i < player->lives; ++i){
        std::cout << "❤️ ";
    }
    std::cout << " Food: " << player->get_mouse_size() << " de " << food << "\n";
    std::cout << "----------------------------------------------------\n";

    //definir oq cada item é visualmente
    const std::map<char, std::string> emojis = {
        //colocar o outro tipo de mapa
        {'#', "🌳"},
        {'&', "🐭"},
        {'*', "🧀"},
        {'X', "☠️ "},
        {'M', "🐁"},
        {'@', "🪵"},
        {'%', "🪨"}
    };

    //imprimir caracter por caracter
    for (const auto& line : temp_board) {
        for(const char c : line){
            if(emojis.count(c)){
                std::cout << emojis.at(c);
            }else{
                std::cout << "  ";
            }
        }
        std::cout << "\n";
    }
}

void MouzeSimulation::run_tests() {
    std::cout << "\n=================================================\n";
    std::cout << "              SIMULATION REPORT\n";
    std::cout << "=================================================\n\n";
    
    // 1. Imprime os parâmetros da simulação
    std::cout << "[RECEIVED PARAMETERS]\n";
    std::cout << "  > FPS: " << fps << "\n";
    std::cout << "  > Lives: " << lives << "\n";
    std::cout << "  > Foods: " << food << "\n";
    std::cout << "  > Type of AI: '" << player_type << "'\n";


    std::cout << "\n=================================================\n";
    std::cout << "                END OF REPORT\n";
    std::cout << "=================================================\n";
}