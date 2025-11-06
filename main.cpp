#include <SFML/Graphics.hpp>
#include "include.h"

int main() {
    VideoMode mode = VideoMode::getDesktopMode();
    RenderWindow window(mode, "Janela de teste", Style::Fullscreen);
    window.setFramerateLimit(60);

    Player p1(100.f, 200.f);
    
    RectangleShape fig({100.f, 100.f});
    fig.setFillColor(Color(146, 49, 176));
    
    // Loop de lógica
    while(window.isOpen()) {

        // Decta eventos no ultimo loop
        Event e;        
        while(window.pollEvent(e)) {
            if(e.type == Event::Closed || (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)) 
                window.close();
            
            if(e.type == Event::KeyPressed) {
       
                switch (e.key.code) {
                    case Keyboard::W :
                        p1.y--; break;
                    case Keyboard::A :
                        p1.x--; break;
                    case Keyboard::S :
                        p1.y++; break;
                    case Keyboard::D :
                        p1.x++; break;
                    default:
                        break;
                }
            }

            
        }
        fig.setPosition(p1.x, p1.y);

        window.clear();
        window.draw(fig);
        window.display();
    }
}