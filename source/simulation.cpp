#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "simulation.hpp"
#include "level.hpp"
#include "output.hpp"
#include "mouse.hpp"
#include "player.hpp"

/**
 * @brief Returns the single instance of the simulation using the Singleton pattern.
 *
 * Ensures that only one instance of the `SnazeSimulation` class exists during the program's execution.
 *
 * @return Reference to the single instance of `SnazeSimulation`.
 */

MouzeSimulation& MouzeSimulation::instance() {
    static MouzeSimulation instance;
    return instance;
}

MouzeSimulation::MouzeSimulation() : game_state(GameState::START){
    fps = 300;
    lives = 5;
    food = 10;
    player_type = "backtracking";
}

void MouzeSimulation::parse_config(const std::string& filename){

    std::ifstream config_file (filename);

    if(!config_file.is_open()){
        std::cout<<"\nError: Could not open the file.\n" << "Using default or insered game settings.\n\n";
        return;
    }
    
    std::string line;

    std::unordered_map<std::string, std::string>config_game;

    while(std::getline(config_file, line)){
        trim(line);
        if(line.empty() || line[0]=='#'){
            continue;
        }

        size_t equals_pos = line.find('=');
        if(equals_pos != std::string::npos){
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);
            trim(key);
            trim(value);

            //limpando aspas duplas
            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }           
            config_game[key]=value;
        }
    }

    // Seção [Game]
    if (config_game.count("fps"))     fps = std::stoi(config_game["fps"]);
    if (config_game.count("lives"))   lives = std::stoi(config_game["lives"]);
    if (config_game.count("food"))    food = std::stoi(config_game["food"]);
    if (config_game.count("playertype"))  player_type = config_game["playertype"];

}

void MouzeSimulation::initialize(int argc, char* argv[]){
    
   for(size_t i=1; i<(size_t)argc; i++){
    std::string arg=argv[i];

    if(arg=="-h" || arg=="--help"){
        help_screen();
        exit(1);
    }
    else if(arg=="--fps"){
        if (i + 1 >= (size_t)argc) {
            help_screen("After --fps there must be integer value."); //caso não seja informado o tamanho após o comando 
            exit(1);
        }
        std::string next_arg = argv[i + 1];
        fps=std::stoi(next_arg); //atribui o valor a variavel 
        ++i;
    }
    else if(arg=="--food"){
        if (i + 1 >= (size_t)argc) {
            help_screen("After --food there must be integer value."); 
            exit(1);
        }
      
        std::string next_arg = argv[i + 1];
        food=std::stoi(next_arg);
        ++i;
    }
    else if(arg=="--playertype"){
        if (i + 1 >= (size_t)argc) {
            help_screen("You must put a random, backtracking or A* playertype!"); 
            exit(1);
        }
      
        std::string next_arg = argv[i + 1];
        player_type=next_arg;
        ++i;
    }
    else if(arg=="--lives"){
        if (i + 1 >= (size_t)argc) {
            help_screen("After --lives there must be integer value."); 
            exit(1);
        }
      
        std::string next_arg = argv[i + 1];
        lives=std::stoi(next_arg);
        ++i;
    }
    else if(ends_with(arg,".dat")){
        level_filename=arg;
    }
    else if(ends_with(arg, ".ini")){
        config_filename=arg;
    }
    else{
        if(!std::filesystem::is_regular_file(level_filename)){
            help_screen("This file is invalid.\n");
            exit(1);
        }
        else{
            help_screen("This argument is invalid.\n");
            exit(1);
        }
    }
   }

    
    if(level_filename.empty()){
        help_screen("Input file not specified. You must provide an initialization file.");
        exit(1);
    }else{
        open_process_file();
    }

    if (!config_filename.empty()) {
        parse_config(config_filename);
    } else {
        std::cout << "File '.ini' not provided. Using default settings.\n";
    }
}

void MouzeSimulation::process_events(){
    if(game_state==START){
        std::cout<<"\n----WELCOME TO THE MOUZE GAME!----\n";
    }
    else if(game_state==WELCOME){
        //pressionar enter
        std::string line;
        std::getline(std::cin, line);
        player = std::make_unique<Player>(levels[current_level_idx]);
        player->lives = lives;
    }
    else if(game_state == LOAD_LEVEL){
    
        if(current_level_idx < levels.size()){

            Level& current_level = levels[current_level_idx]; //pegando o nível 

            if(initial_level){
                current_level.find_start_position();
            }

            render_board(current_level);
            // Gera a comida 
            current_level.generate_food();
        }

        const Level& current_level = levels[current_level_idx];
        
        if(initial_level){
            levels[current_level_idx].current_mouse = levels[current_level_idx].start_mouse;
            head_mouse = levels[current_level_idx].start_mouse;
            //reiniciar o indice do caminho p rodar novo caminho 
            idx_path = 0;
            
            //imprimir level inicial
            levels[current_level_idx].reset_level(initial_level);
            std::this_thread::sleep_for(std::chrono::milliseconds(fps));
            render_board(current_level); //labirinto
            initial_level = false;
            search_food = true;
        }

        
        //imprimir antes de mudar de lugar e depois de identificar onde é o ponto de spaw
        levels[current_level_idx].fill_data(head_mouse, dead);
        std::this_thread::sleep_for(std::chrono::milliseconds(fps));
        render_board(current_level); //labirinto
    }else if(game_state == GameState::THINKING){
        //limpar os dados p ele n ficar preso
        clear_actions();

        const Level& current_level = levels[current_level_idx];

        if(player_type=="backtracking" ){
            if(search_food || idx_path >= path_execute.size()){
                player->computed_path_bt(head_mouse);
                path_execute.clear();
                path_execute = player->path;
                idx_path = 0;
                search_food = false;
            }
               
            if(idx_path < path_execute.size()){
                Point next_head_snake = path_execute[idx_path];
                //definir direção atual da cobra
                direction_snake = player->path_direction[idx_path];
                ++idx_path;
                
                char next_cell = level.get_cell(current_level, next_head_snake);

                if (level.is_empty_cell(next_cell)) {
                    has_none = true;
                    head_mouse = next_head_snake;
                } else if (level.is_food(next_cell)) {
                    has_food = true;
                    head_mouse = next_head_snake;
                    levels[current_level_idx].current_mouse = head_mouse;
                    levels[current_level_idx].update_board_after_food(); // para limpar a comida
                    idx_path = 0;
                    search_food = true;
                } else {
                    has_wall = true;
                    dead = true;
                }
            }
        }else if(player_type == "A*"){

        }else{     
            Point next_head_snake = player->computed_random(head_mouse);
            direction_snake = player->get_direction();

            char next_cell = level.get_cell(current_level, next_head_snake);

            if(level.is_empty_cell(next_cell)){
                has_none = true;
                head_mouse = next_head_snake;
            }else if(level.is_food(next_cell)){
                has_food = true;
                head_mouse = next_head_snake;
            }else{
                has_wall = true;
                dead = true;
            }
        }
        //pontuação por comer ou por andar sem bater
        if(has_food){
            player->score += 100;
        }
        if(has_none){
            player->score += 5;
        }
    }else if(game_state == GameState::RUNNING){
        levels[current_level_idx].reset_level(initial_level);
        levels[current_level_idx].fill_data(head_mouse, dead);
        std::this_thread::sleep_for(std::chrono::milliseconds(fps));
        render_board(levels[current_level_idx]);
        dead = false;
    }else if(game_state == GameState::EATING){
        player->increase_mouse_size();
    }else if(game_state == GameState::CRASHED){
        initial_level = true;//p cabeça voltar p ponto de spaw
        direction_snake = {Dir::N};
        //pressionar enter
        std::string line;
        std::getline(std::cin, line);
        --player->lives;
        levels[current_level_idx].current_mouse = levels[current_level_idx].start_mouse;
        reset_food();
        //quando morre o corpo vai pro inicio junto da cabeça dela
    }else if(game_state == GameState::LEVEL_UP){
        player->score += 250;
        int aux_score = player->score;
        int aux_lives = player->lives;

        initial_level = true; //cabeça ir pro novo ponto de spaw

        ++current_level_idx;
        //mudar o nível do player e restaurar informações dele
        player = std::make_unique<Player>(levels[current_level_idx]);
        player->score = aux_score;
        player->lives = aux_lives;

        if(current_level_idx<levels.size()){
            std::cout<<"\n Press <ENTER> for the next level.\n";
            //pressionar enter
            std::string line;
            std::getline(std::cin, line);
            has_level=true;
        }else{
            has_level = false;
        }
    }else if(game_state == GameState::LOST){
        std::cout<<"\nYOU LOST THE GAME!\n";
    }else if(game_state == GameState::WON){
        std::cout<<"\nYOU WIN THE GAME!\n";
    }
}

void MouzeSimulation::update(){
    if(game_state==GameState::START){
        game_state=GameState::WELCOME;
    }else if (game_state==GameState::WELCOME){
        game_state= GameState::LOAD_LEVEL; 
    }else if(game_state==GameState::LOAD_LEVEL){
        game_state = GameState::THINKING;
    }else if(game_state==GameState::THINKING){
        game_state = GameState::RUNNING;
    }else if(game_state == GameState::RUNNING){
        if(has_food){
            game_state = GameState::EATING;
        }else if(has_wall){
            game_state= GameState::CRASHED;
        }else if(has_none){
            game_state=GameState::THINKING;
        }
       
    }else if(game_state == GameState::EATING){
        if(is_full_food()){
            game_state = GameState::LEVEL_UP;
        }else{
            game_state = GameState::LOAD_LEVEL;
        }
    }else if(game_state == GameState::CRASHED){
        if(player->lives > 0){
            game_state=GameState::LOAD_LEVEL;
        }else{
            game_state=GameState::LOST;
        }
    }else if(game_state == GameState::LEVEL_UP){
        if(has_level){ 
            game_state = GameState::LOAD_LEVEL;

        }
        else{ 
            game_state=GameState::WON;
        }   
    }else if(game_state == GameState::WON){
        game_state=GameState::END; 
    }else if(game_state==GameState::LOST){
        game_state=GameState::END;
    }
}

void MouzeSimulation::render(){
    if(game_state==WELCOME){
        std::cout << "\n Press <ENTER> for continue.\n";
    }
    else if(game_state==LOAD_LEVEL){
    }
    else if(game_state==LEVEL_UP){
        std::cout<<"\nLevel "<< current_level_idx + 1 << " completed!\n";
    }else if(game_state==CRASHED){
        if(has_wall){
            std::cout<<"You hit the wall!";
        }
        else{
            std::cout<<"You hit your own body!";
        }
        std::cout << "\n Press <ENTER> for continue.\n";
    }
    else if(game_state==WON){
        std::cout<<"\nCONGRATULATIONS! YOU ATE ALL THE FOOD!\n";
    }
    else if(game_state==LOST){
        std::cout<<"\nYOU LOST ALL YOUR LIVES...\n";
    }
    else if(game_state==END){
        std::cout<<"\n--END GAME--\n";
    }
}

void MouzeSimulation::open_process_file(){
    std::ifstream level_file(level_filename);

    if(!level_file.is_open()){
        std::cout<<"\nError: Could not open this file.\n";
        exit(1);
    }

    while(level_file >> rows >> cols){
        if(rows <= 0 || cols<=0 || rows>100 || cols > 100 ){
            std::cout<<"\nINVALID DIMENSIONS!\n";
            exit(1);
        }

        // Ignora o resto da linha de dimensões para começar a leitura do tabuleiro
        level_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        Level current_level;
        current_level.rows = rows;
        current_level.cols = cols;
        current_level.board.resize(rows);

        int spawn_point_count = 0; //quantidade de &
        bool valid_nivel=true;

        for (int i = 0; i < rows; ++i) {
                std::getline(level_file, current_level.board[i]);
                // Conta os pontos de spawn na linha 
                for (char c : current_level.board[i]) {
                    if (c == '&') {
                        spawn_point_count++;
                    }

                    if (!(c == '#' ||c == '@' || c == '%' || c == '&' || c == ' ' || c=='.')) {
                        std::cout << "Error: Invalid character '" << c << "' found." << std::endl;
                        std::cout << "Level ignored because it contains symbols that are not allowed." << std::endl;
                        valid_nivel = false;
                        break;
                    }
                }
                if(!valid_nivel){
                    break;
                }
               
            }
        
            // Valida o conteúdo do tabuleiro
            if (spawn_point_count > 1) {
                std::cout << "Warning: The level "<<rows<< "x" <<cols<<" contains more than one '&'." << std::endl;
                // Continua para o próximo nível no arquivo, se houver
                continue;
            }
            
            if(spawn_point_count==0){
                std::cout<<"Warning: The level "<<rows<<"x"<<cols<<" does not contain a spawn point ('&')."<<std::endl;
                continue;
            }
            
            //Se tudo estiver correto, adiciona o nível à lista
            levels.push_back(current_level);
            std::cout << "Info: Level of " << rows << "x" << cols << " loaded successfully." << std::endl;
        }


      if(levels.empty()){
        std::cout << "Error: No valid levels found in the file." << std::endl;
        exit(1);
      }
}  

/**
 * @brief Trims whitespace from the beginning and end of a string.
 *
 * Modifies the string passed by reference, removing any leading and trailing 
 * whitespace characters.
 *
 * @param s The string to be modified by trimming its ends.
 */

void MouzeSimulation::trim(std::string& s) {
    // Apaga os espaços do final 
    while (!s.empty() && std::isspace(s.back())) {
        s.pop_back(); // Remove o último caractere
    }

    // Apaga os espaços do início 
    while (!s.empty() && std::isspace(s.front())) {
        s.erase(0, 1); // Remove o primeiro caractere
    }
}

/**
 * @brief Removes the current food from the current level's board.
 *
 * Marks the food's position on the board as empty, clearing the space it occupied
 * so that new food can be generated or the space can be updated.
 */

void MouzeSimulation::reset_food(){
    Point clear = levels[current_level_idx].food_mouse;
    levels[current_level_idx].board[clear.x][clear.y] = ' ';
}

/**
 * @brief Checks if the snake has eaten all the food required to complete the level.
 *
 * Compares the snake's current size with the total amount of food that must be consumed.
 *
 * @return true if the snake's size has reached or exceeded the total food amount, false otherwise.
 */

bool MouzeSimulation::is_full_food(){
    if(player->get_mouse_size()  >= food){
        return true;
    }else{
        return false;
    }
}

/**
 * @brief Checks if a string ends with the specified suffix.
 *
 * Compares the end of the string `str` with the `suffix` and returns true
 * if `str` ends with `suffix`, and false otherwise.
 *
 * @param str The main string to be checked.
 * @param suffix The suffix to search for at the end of the main string.
 * @return true if str ends with suffix, false otherwise.
 */

bool MouzeSimulation::ends_with(const std::string& str, const std::string& suffix) {
    if (str.size() < suffix.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

/**
 * @brief Checks if the simulation has finished.
 *
 * Returns true if the current game state is END, indicating that the simulation
 * has reached its conclusion.
 *
 * @return true if the game is over, false otherwise.
 */

bool MouzeSimulation::is_over() const{
    if(game_state == GameState::END){
        return true;
    }
    return false;
}

/**
 * @brief Resets the game's action and event indicators.
 *
 * Clears the flags that indicate if the snake has eaten food, hit a wall,
 * moved to an empty cell, or collided with its own body.
 */

void MouzeSimulation::clear_actions(){
    has_food = false;
    has_wall = false;
    has_none = false;
}
