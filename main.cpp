#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <thread>

using namespace std;
using namespace sf;

void rendering(RenderWindow* window) {
    window->setActive(true);

    // The rendering loop
    while (window->isOpen()) {
        
        // draw...

        // end the current frame
        window->display();
    }
}


int main() {

    VideoMode mode = VideoMode::getDesktopMode();
    RenderWindow window(mode, "Janela de teste", Style::Fullscreen);

    window.setActive(false);

    thread thread(&rendering, &window);
    
    // Loop de lógica
    while(window.isOpen()) {

        // Decta eventos no ultimo loop
        Event e;        
        while(window.pollEvent(e)) {
            if(e.type == Event::Closed || (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)) {
                window.close();
            }
        }
    }
}