#ifndef FOOD_H_
#define FOOD_H_

#include <utility>

using namespace std;

class Food {
private:
    pair<int,int> pos;
public:
    // Definições vazias e padrão para construtores e destrutores
    Food() = default;
    Food(int gridW, int gridH);
    ~Food() = default;

    // Funcoes auxiliares
    int getX() const { return pos.first; } // Obtém posição X
    int getY() const { return pos.second; } // Obtém posição Y
    void setPos(int x, int y) { pos = {x,y}; }  // Define uma nova posição
    void draw(int cellSize) const;  // Faz a rendereização
};

#endif