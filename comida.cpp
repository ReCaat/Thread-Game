#include "comida.h"
#include <raylib.h>
#include <cstdlib>

Food::Food(int gridW, int gridH) {
    // Spawna uma comida dentro dos limites do mapa
    pos.first = GetRandomValue(0, gridW - 1);
    pos.second = GetRandomValue(0, gridH - 1);
}

void Food::draw(int cellSize) const {
    DrawRectangle(pos.first * cellSize, pos.second * cellSize, cellSize, cellSize, RED);
}
