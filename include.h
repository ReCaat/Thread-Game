#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;
using namespace sf;

class Player {
public:
    float x;
    float y;

    Player(float a, float b) {
        x = a;
        y = b;
    }
};