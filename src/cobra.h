#ifndef SNAKE_H_
#define SNAKE_H_

#include <deque>
#include <vector>
#include <utility>
#include "comida.h"

using namespace std;

class Snake {
private:
    bool playerType; // 0 = player1 (WASD), 1 = player2 (setinhas)
    int length;
    deque<pair<int,int>> body; // head é front do deque
    int direction; // 0 up,1 down,2 left,3 right

public:
    Snake() = default;
    Snake(int pos_x, int pos_y, bool player, int init_size);

    // getters
    deque<pair<int,int>>& get_body(); // referencia para corpo

    int get_size(); // retorna tamanho da cobra

    bool is_player() const { return playerType; }

    // movimento
    void set_direction(int d); // Função que coloca as regras pra direção da cobra (não pode virar 180)

    void scan_inputs(); // Função pra ler o que tao apertando

    pair<int,int> next_head() const; // Função que calcula a nova head da cobrinha

    void apply_move(bool grow);          // Função que aplica o movimento

    // Checks
    bool check_wall(int gridW, int gridH) const;
    bool check_self_collision() const;
    bool check_enemy_collision(Snake &enemy) const;

    // Função de comer comida, retira uma comida do vetor de comida
    bool consume_food(vector<Food>& foods);

    // Função pra desenhar cobrinha
    void draw(int cellSize) const;

    // Função que reseta cobrinha
    void reset(int pos_x, int pos_y, int init_size);
};

#endif
