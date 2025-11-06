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

mutex currentPos;

void logicThread(Player *p1) {
    
}

int main() {
    VideoMode mode = VideoMode::getDesktopMode();
    RenderWindow window(mode, "Janela de teste", Style::Fullscreen);
    
    Player p1(100.f, 200.f);
    
    thread thread(&logicThread, &p1);

    CircleShape circulo(100.f);
    circulo.setFillColor(Color(146, 49, 176));
    circulo.setPosition(200.f, 100.f);
    
    // Loop de lógica
    while(window.isOpen()) {

        // Decta eventos no ultimo loop
        Event e;        
        while(window.pollEvent(e)) {
            if(e.type == Event::Closed || (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)) 
                window.close();
            
        }

        window.clear();
        window.draw(circulo);
        window.display();
    }
}