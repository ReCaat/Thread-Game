#include <SFML/Graphics.hpp>
#include "Window/Mouse.hpp"
#include "include.h"

int main() {
    // Esta função gera a janela principal do jogo
    RenderWindow window(VideoMode(VideoMode::getDesktopMode()), "Jogo");
    
    // Gera forma de circulo com raio 100px e coloca a cor para verde
    CircleShape shape(100.f);
    shape.setFillColor(Color::Green);

    // Loop de funcionamento da janela
    while(window.isOpen()){

        // Loop que verifica se o botão de fechar foi clicado
        while(const optional event = window.pollEvent()){
            // Se for clicado, fecha janela
            if(event->is<Event::Closed>()){
                window.close();
            }

            if(const auto* check_MousePress = event->is<Event::MouseButtonPressed()>){

            }
        }

        // Limpa conteudo anterior
        window.clear();

        // Desenha o circulo na janela
        window.draw(shape);

        // Carrega na tela o conteudo renderizado
        window.display();
    }
}