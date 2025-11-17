#ifndef OBJ_H_
#define OBJ_H_

#include <SFML/Graphics.hpp>
#include "include.h"

class Tower{
private:
    pair<double,double> size;
    pair<double,double> position;
    Color color;

public:
    Tower(double pos_x, double pos_y){
        this->position.first = pos_x;
        this->position.second = pos_y;

        this->size.first = 10.0;
        this->size.second = 10.0;

        this->color = Color::Green;
    }

    
};

#endif
