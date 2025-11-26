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
    int getX() const { return pos.first; }
    int getY() const { return pos.second; }
    void setPos(int x, int y) { pos = {x,y}; }
    void draw(int cellSize) const;
};

#endif