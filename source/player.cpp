#include "player.hpp"

#include <iostream>


/**
 * @brief Checks whether a given position is a valid cell for the snake to move to.
 * 
 * @param p Point to check.
 * @return True if the position is within bounds and not a wall or snake body.
 */

bool Player::is_valid(const Point& p) const{
    if(p.x >= 0 && p.y >= 0 && p.x < level.rows && p.y < level.cols && level.board[p.x][p.y] != '#'){
        return true;
    }
    return false;
}


/**
 * @brief Computes a valid path to the food using backtracking.
 * 
 * Starts from the snake's head and explores valid paths using a stack.
 * If a path to the food is found, it stores the path and directions.
 * If no path is found, it chooses a random valid direction.
 * 
 * @param head_mouse Current position of the snake's head.
 */

void Player::computed_path_bt(Point head_mouse){
    path.clear();
    path_direction.clear();
    path_valid = false;

    std::stack<PathUnit> place_to_visit;
    std::set<Point> visited;

    //ponto de inicio
    place_to_visit.push({head_mouse, {}});

    while (!place_to_visit.empty()) {
        PathUnit current = place_to_visit.top();
        place_to_visit.pop();

        Point current_pos = current.current_pos;

        if (visited.count(current_pos)) continue;
        visited.insert(current_pos);
        if (level.board[current_pos.x][current_pos.y] == '*') {
            //armazenar o caminho e a direção até a comida
            Point step = head_mouse; 
            for (Dir direction : current.path) {
                //preencher vetor de direções p direcionar a cabeça
                path_direction.push_back(direction);
                step.x += MOVES[static_cast<int>(direction)].x;
                step.y += MOVES[static_cast<int>(direction)].y;
                path.push_back(step);
            }
            path_valid = true;
            return;
        }

        for (int i = 0; i < 4; ++i) {
            Dir d = static_cast<Dir>(i);
            Point next_pos{
                current_pos.x + MOVES[i].x,
                current_pos.y + MOVES[i].y
            };

            if (is_valid(next_pos) && !visited.count(next_pos)) {
                PathUnit next_unit = current;
                next_unit.current_pos = next_pos;
                next_unit.path.push_back(d);
                place_to_visit.push(next_unit);
            }
        }
    }

    if(not path_valid){
        Point random_pos = computed_random(head_mouse);
        //definir a direção da cobra aleatória preenchendo o vetor
        path_direction.push_back(get_direction());
        path.push_back(random_pos);
        path_valid = true;
    }   
}



/**
 * @brief Checks if there is any path computed for the snake.
 * 
 * @return True if a path exists, false otherwise.
 */

bool Player::has_path() const{
    return !path.empty();
}


/**
 * @brief Returns whether the last computed path is valid.
 * 
 * @return True if the computed path is valid, false otherwise.
 */

bool Player::get_valid_path() const{
    return path_valid;
}

/**
 * @brief Increases the size of the snake by one unit.
 */

void Player::increase_mouse_size() {
    ++m_mouse.mouse_size;
}

/**
 * @brief Gets the current size of the snake.
 * 
 * @return The number of segments in the snake.
 */

size_t Player::get_mouse_size() const {
    return m_mouse.mouse_size;
}


/**
 * @brief Calculates the next head position based on the given direction.
 * 
 * @param head_position Current head position.
 * @param next_direction Direction to move.
 * @return The new head position after the move.
 */

Point Player::get_next_head_position(Point head_position, Dir next_direction) const {
        
        Point new_head = head_position;  // Pega a posição da cabeça atual(sempre pega a inicial onde a cobra nasce)
       
        switch (next_direction) {
            case Dir::N:  new_head.x--; break;
            case Dir::S:  new_head.x++; break;
            case Dir::L:  new_head.y++; break;
            case Dir::O:  new_head.y--; break;
            default: break;
        }
        return new_head;
}


/**
 * @brief Chooses a random valid direction for the snake to move.
 * 
 * Checks all directions, filters valid ones, and randomly picks one.
 * If no direction is valid, chooses from all directions.
 * 
 * @param head_mouse Current head position.
 * @return The next position in the chosen direction.
 */

Point Player::computed_random(const Point& head_mouse){
    std::vector<Dir> possible_directions;
 
    // Testa todas as direções
    for (Dir dir : {Dir::N, Dir::S, Dir::L, Dir::O}) {
        Point new_pos = get_next_head_position(head_mouse, dir);
        if (is_valid(new_pos)) {
            possible_directions.push_back(dir);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    // Nenhuma direção livre, adiciona todas possíveis
    if (possible_directions.empty()) {
        possible_directions = {Dir::N, Dir::S, Dir::L, Dir::O};
    }

    // Embaralha as direções 
    std::shuffle(possible_directions.begin(), possible_directions.end(), gen);
    //Pega a primeira posição do vetor
    Dir chosen_direction = possible_directions.front();
    direction_head = chosen_direction;

    return get_next_head_position(head_mouse, chosen_direction);
}

/**
 * @brief Returns the last direction chosen or used by the snake.
 * 
 * @return The current direction of the snake's head.
 */

Dir Player::get_direction(){
    return direction_head;
}

//Busca A* e suas funções necessárias
void Player::computed_path_A(Point head_mouse) {
    path.clear();
    path_valid = false;

    const int rows = level.rows;
    const int cols = level.cols;

    Point start = head_mouse;
    Point goal;
    bool found_goal = false;

    // CORREÇÃO DE INDEXAÇÃO: Usando [j][i] para corresponder a [x][y]
    for (int i = 0; i < rows && !found_goal; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (level.board[i][j] == '*') {
                goal = {i, j};
                found_goal = true;
                break;
            }
        }
    }

    if (!found_goal) {
        return;
    }

    using State = std::tuple<int, Point>; // Simplificado: Apenas prioridade e ponto
    std::priority_queue<State, std::vector<State>, std::greater<>> open;
    
    std::unordered_map<Point, Point> came_from;
    std::unordered_map<Point, int> cost_so_far;

    auto heuristic = [](Point a, Point b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
    };

    open.emplace(heuristic(start, goal), start);
    came_from[start] = start;
    cost_so_far[start] = 0;

    std::vector<Point> moves = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    while (!open.empty()) {
        auto [priority, current] = open.top();
        open.pop();

        if (current == goal) {
            path_valid = true;
            break;
        }

        for (auto move : moves) {
            Point next = {current.x + move.x, current.y + move.y};

            if (!is_valid(next)) {
                continue;
            }

            int terrain_cost = get_terrain_cost(next);
            int new_cost = cost_so_far[current] + terrain_cost;

            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                int new_priority = new_cost + heuristic(next, goal);
                open.emplace(new_priority, next);
                came_from[next] = current;
            }
        }
    }
    
    if (path_valid) {
        Point current = goal;
        while (current != start) {
            path.push_back(current);
            current = came_from.at(current);
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }
}

int Player::get_terrain_cost(Point p) {
    // Make sure the point is valid before checking the board
    if (p.y < 0 || p.y >= level.rows || p.x < 0 || p.x >= level.cols) {
        return 9999; 
    }

    char tile = level.board[p.y][p.x]; // Standard [row][col] access
    switch (tile) {
        case '@': return 10; // High difficulty
        case '%': return 5;  // Medium difficulty
        default:  return 1;  // Normal ground
    }
}