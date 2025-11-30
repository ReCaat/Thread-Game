#include "cobra.h"
#include <raylib.h>
#include <algorithm>

Snake::Snake(int pos_x, int pos_y, bool player, int init_size)
    : playerType(player), length(init_size), direction(3)
{
    body.clear();
    for (int i = 0; i < init_size; ++i) {
        body.push_back(make_pair(pos_x - i, pos_y));
    }
}

deque<pair<int,int>>& Snake::get_body() {
    return body;
}

int Snake::get_size(){
    return length;
}

void Snake::set_direction(int d){
    // Funções garantem que a cobrinha não vira pra tras
    // e não anda por cima do seu corpo
    if ((d == 0 && direction == 1) || (d == 1 && direction == 0) ||
        (d == 2 && direction == 3) || (d == 3 && direction == 2)) {
        return;
    }
    direction = d;
}

void Snake::scan_inputs(){
    // Se playerType = 0 (player 1), usa WASD, se player = 1 (player 2), usa setinhas
    if (!playerType) {
        if (IsKeyPressed(KEY_W)) set_direction(0);
        if (IsKeyPressed(KEY_S)) set_direction(1);
        if (IsKeyPressed(KEY_A)) set_direction(2);
        if (IsKeyPressed(KEY_D)) set_direction(3);
    } else {
        if (IsKeyPressed(KEY_UP)) set_direction(0);
        if (IsKeyPressed(KEY_DOWN)) set_direction(1);
        if (IsKeyPressed(KEY_LEFT)) set_direction(2);
        if (IsKeyPressed(KEY_RIGHT)) set_direction(3);
    }
}

pair<int,int> Snake::next_head() const {
    // Calcula a proxima posição da cabeça
    auto head = body.front();
    if (direction == 0) return { head.first, head.second - 1 };
    if (direction == 1) return { head.first, head.second + 1 };
    if (direction == 2) return { head.first - 1, head.second };

    // direção "padrão" = 3
    return { head.first + 1, head.second };
}

void Snake::apply_move(bool grow) {
    auto nh = next_head();
    body.push_front(nh);
    if (!grow) {
        body.pop_back();
    } else {
        ++length;
    }
}

bool Snake::check_wall(int gridW, int gridH) const {
    auto head = body.front();
    return head.first < 0 || head.first >= gridW || head.second < 0 || head.second >= gridH;
}

bool Snake::check_self_collision() const {
    auto head = body.front();
    // passa pelo corpo da cobrinha sequencialmente verificando.
    // Começa no 1 para não contar colisão head / head
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head) return true;
    }
    return false;
}

bool Snake::check_enemy_collision(Snake& enemy) const {
    auto head = body.front();
    const auto& eb = enemy.get_body();

    for (size_t i = 0; i < eb.size(); i++){
        if (eb[i] == head) return true;
    }
    return false;
}

bool Snake::consume_food(vector<Food>& foods) {
    auto head = body.front();
    for (int i = 0; i < (int)foods.size(); ++i) {
        if (foods[i].getX() == head.first && foods[i].getY() == head.second) {
            foods.erase(foods.begin() + i);
            return true;
        }
    }
    return false;
}

void Snake::draw(int cellSize) const {
    if(playerType == 0){
        for (auto &p : body) {
            DrawRectangle(p.first * cellSize, p.second * cellSize, cellSize, cellSize, GREEN);
        }
    }
    else if (playerType == 1){
        for (auto &p : body) {
            DrawRectangle(p.first * cellSize, p.second * cellSize, cellSize, cellSize, BLUE);
        }
    }
}

void Snake::reset(int pos_x, int pos_y, int init_size) {
    body.clear();
    length = init_size;
    for (int i = 0; i < init_size; ++i) {
        body.push_back(make_pair(pos_x - i, pos_y));
    }
    direction = 3;
}

