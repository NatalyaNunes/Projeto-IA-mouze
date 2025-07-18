#include "level.hpp"

#include <vector>
#include <random>


/**
 * @brief Finds the snake's starting position in the level.
 * 
 * Iterates through the board to locate the '&' character that marks
 * the initial position of the snake and stores it.
 *
 * @details This function sets the value of `start_snake` with the coordinates
 * of the starting point.
 */

void Level::find_start_position(){
    for(int i=0;i<rows;++i){
        for(int j=0;j<cols;++j){
            if(board[i][j] == '&'){
                start_mouse = {i, j};
            }else if(board[i][j] == '@'){
                Point m_dif = {i, j};
                medium_dificulty.push_back(m_dif);
            }else if(board[i][j] == '%'){
                Point h_dif = {i, j};
                medium_dificulty.push_back(h_dif);
            }
        }
    }
}


/**
 * @brief Generates food on a random empty space of the board.
 * 
 * Scans the board for empty spaces and places the food marker '*' at
 * a randomly chosen available location.
 *
 * @details Uses a random number generator to select the position.
 */

void Level::generate_food(){
    //verificando espaços vazios
    std::vector<Point> empty_spaces;
    for(int i=0;i<rows;++i){
        for(int j=0;j<cols;++j){
            if(board[i][j] == ' '){
                empty_spaces.push_back({i, j});
            }
        }
    }

    //gerando a comida aleatoriamente em um espaço vazio do tabuleiro verificado anteriormente
    if(!empty_spaces.empty()){
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, empty_spaces.size() - 1);

        // Escolhe um índice aleatório da nossa lista de locais vazios
        int random_index = distribution(generator);
        
        // Pega a coordenada correspondente
        food_mouse = empty_spaces[random_index];

        //Coloca a comida no tabuleiro
        board[food_mouse.x][food_mouse.y] = '*'; // Usando '*' para representar a comida
    }

}


/**
 * @brief Resets the level's board to its initial state.
 * 
 * Clears non-wall, non-invisible wall, and non-food cells.
 * If not the initial level, also removes the snake's start marker '&'.
 *
 * @param initial_level Indicates if it's the initial level setup.
 */

void Level::reset_level(bool initial_level){
    for(int i=0;i<rows;++i){
        for(int j=0;j<cols;++j){
            if(board[i][j] != '#' && board[i][j] != '@' && board[i][j] != '%' && board[i][j] != '.' && board[i][j] != '*'){
                board[i][j] = ' ';
            }
            if(initial_level == false && board[i][j] =='&'){
                board[i][j] = ' ';
            }
        }
    }
    if(initial_level){
        board[start_mouse.x][start_mouse.y] = '&';
   }
}

/**
 * @brief Fills the board with the current state of the snake.
 * 
 * Marks the head of the snake (alive or dead), body segments,
 * and the direction the snake is facing.
 *
 * @param head Position of the snake's head.
 * @param body Vector containing the body segments.
 * @param dead Whether the snake is dead.
 * @param direction_snake Current direction of the snake.
 */

void Level::fill_data(Point head, bool dead){
    for(int i=0;i<rows;++i){
        for(int j=0;j<cols;++j){
            if(head.x == i && head.y == j){
                if(dead){
                    board[i][j] = 'X';//morreu
                }else{
                    board[i][j] = 'M';
                }
            }
        }
    }
}

/**
 * @brief Gets the character stored in a given board position.
 * 
 * @param level Reference to the level.
 * @param p Coordinates of the cell.
 * @return Character at the specified position.
 */

char Level::get_cell(const Level& level, const Point& p) {
    return level.board[p.x][p.y];
}


/**
 * @brief Checks whether a cell is considered empty.
 * 
 * @param cell Character representing a cell.
 * @return True if the cell is empty or has a temporary marker, false otherwise.
 */

bool Level::is_empty_cell(char cell) {
    return cell == ' ' || cell == 'C';
}


/**
 * @brief Checks whether a cell contains food.
 * 
 * @param cell Character representing a cell.
 * @return True if the cell contains food, false otherwise.
 */

bool Level::is_food(char cell) {
    return cell == '*';
}

/**
 * @brief Clears the previous food from the board.
 * 
 * Removes the food character from the snake's head position after it eats.
 */

void Level::update_board_after_food() {
    // remove a comida anterior da cabeça da cobra
    board[current_mouse.x][current_mouse.y] = ' ';
}