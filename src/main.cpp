#include <raylib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>

#include "semaforo.h"
#include "comida.h"
#include "cobra.h"

using namespace std;

// Configs do grid
constexpr int GRID_W = 40;
constexpr int GRID_H = 30;
constexpr int CELL = 20;

// Presets do jogo
constexpr int SPAWN_MS = 1000;      // delay de spawn de comida = 1000ms
constexpr int MOVE_MS = 200;        // tick do movimento da cobra em ms
constexpr int MAX_FOODS = 10;        // limite de comidas na tela simultaneamente
constexpr int MAX_SPAWN_TRIES = 50;  // tentativas de spawnar evitando as cobras

// Recursos compartilhados
vector<Food> foods;      // Lista de comidas
mutex foods_mtx;     // Mutex das comidas

// Cobras são inicializadas verticalmente no centro da tela
// e horizontalmente em 1/4 e 3/4 da tela respectivamente
Snake snake1(GRID_W/4, GRID_H/2, 0, 3);    // Inicializa cobra1 com 3 de tamanho
Snake snake2(3*GRID_W/4, GRID_H/2, 1, 3);     // Inicializa cobra2 com 3 de tamanho
mutex snake1_mtx;      // Mutex da cobrinha1
mutex snake2_mtx;         // Mutex da cobrinha 2

// Semaforo compartilhado
Semaphore arena_sem(1);

atomic<bool> running(true);

// spawna comida evitando o corpo das cobras
void spawn_food_avoid_snakes() {
    // Loop que spawna 1 comida. O Loop é necessário para tentar novamente
    // caso a posição de spawn seja inválida (há um objeto ali).
    for (int attempt=0; attempt < MAX_SPAWN_TRIES; ++attempt) {
        Food f(GRID_W, GRID_H);
        bool collide = false;

        // checa cobra 1
        {
            // Mutex para acessar as propriedades da cobra 1
            lock_guard<mutex> lk(snake1_mtx);
            // Loop que verifica se a cobra1 está colidindo com a comida spawnada
            for (auto &p : snake1.get_body()){
                if (p.first == f.getX() && p.second == f.getY()){
                    collide = true; 
                    break; 
                }
            }
        }
        // Se detectar colisão, segue para a proxima iteração e não spawna essa comida
        if (collide) continue;

        // checa cobra 2
        {
            // Mutex para acessar as propriedades da cobra 2
            lock_guard<mutex> lk(snake2_mtx);
            // Loop que verifica se a cobra está colidindo com al
            for (auto &p : snake2.get_body()){
                if (p.first == f.getX() && p.second == f.getY()){
                    collide = true; 
                    break;
                }
            }
        }
        // Se detectar colisão, segue para a proxima iteração e não spawna essa comida
        if (collide) continue;

        // Spawna comida
        {
            // Mutex para acessar a lista de comidas
            lock_guard<mutex> lk(foods_mtx);
            // Insere comida spawnada na lista de comidas
            if ((int)foods.size() < MAX_FOODS) foods.push_back(f);
        }
        return;
    }
    // Se não tiver espaço, pula spawn
}

// thread para spawnar comida
void spawner_thread() {
    using namespace chrono;
    // Loop que controla a frequencia de spawn de comida. Quando spawna da sleep pelo tempo
    // de spawnar a próxima
    while (running) {
        this_thread::sleep_for(milliseconds(SPAWN_MS));

        spawn_food_avoid_snakes();
    }
}

// thread para cada tick de cada cobra
void snake_thread(Snake* s, mutex* s_mtx, Snake* e, mutex* e_mtx, int &enemy_score) {
    using namespace chrono;
    while (running) {
        // Coloca a thread para dormir pelo tick de movimento
        this_thread::sleep_for(milliseconds(MOVE_MS));

        // tentativa de acessar escrita na arena
        arena_sem.wait();

        bool died = false;
        bool grew = false;

        // pega os locks dos mutexes
        {
            lock_guard<mutex> lk_sn(*s_mtx);     // Mutex da cobra atual
            lock_guard<mutex> lk_f(foods_mtx);       // Mutex das comidas
            lock_guard<mutex> lk_en(*e_mtx);     // Mutex da cobra inimiga

            // Obtém próxima cabeça da cobra
            auto nh = s->next_head();

            // verifica se bateu na parede
            if (nh.first < 0 || nh.first >= GRID_W || nh.second < 0 || nh.second >= GRID_H) {
                died = true;
            } 
            
            else {
                // verifica se comeu (colisão com comida)
                // Se sim, aumenta o tamanho (flag grew)
                for (int i = 0; i < (int)foods.size(); ++i) {
                    if (foods[i].getX() == nh.first && foods[i].getY() == nh.second) {
                        foods.erase(foods.begin() + i);
                        grew = true;
                        break;
                    }
                }
                // aplica movimento
                s->apply_move(grew);

                // verifica se bate em si mesmo
                // Se sim, morre
                if (s->check_self_collision()){
                    died = true;
                }

                // Verifica se bate no inimigo
                // Se sim, morre
                if(s->check_enemy_collision(*e)){
                    died = true;
                }
            }

            // Verifica se morreu
            if (died) {
                if (s == &snake1)
                    // Reseta posição da cobra 1 se a cobra 1 morreu
                    s->reset(GRID_W/4, GRID_H/2, 3);
                else
                    // Reseta posição da cobra 2 se a cobra 2 morreu
                    s->reset(3*GRID_W/4, GRID_H/2, 3);

                // Aumenta o placar do inimigo
                enemy_score += 1;

                // Limpa as comidas da tela
                foods.clear();
            }
        }

        // Libera o uso de escrita da arena
        arena_sem.signal();
    }
}

int main() {
    InitWindow(GRID_W * CELL, GRID_H * CELL, "Snake Multiplayer Threads");
    SetTargetFPS(60);

    // Spawn de comida inicial
    {
        // Acessa o mutex para a lista de comidas e insere uma nova comida na lista se tiver permissão
        lock_guard<mutex> lk(foods_mtx);
        foods.push_back(Food(GRID_W, GRID_H));
    }

    // placar
    int score1 = 0;
    int score2 = 0;

    // threads
    // Thread que spawna comidas fazendo tentativas até que encontre uma posição válida
    thread spawner(spawner_thread);
    // Thread para a cobrinha 1
    thread t1(snake_thread, &snake1, &snake1_mtx, &snake2, &snake2_mtx, ref(score2));
    // Thread para a cobrinha 2
    thread t2(snake_thread, &snake2, &snake2_mtx, &snake1, &snake1_mtx, ref(score1));

    // Loop do jogo (scan de inputs + desenhar jogo)
    while (!WindowShouldClose()) {
        // INPUT: player1 = WASD, player2 = setinhas
        if (IsKeyPressed(KEY_W)) { lock_guard<mutex> lk(snake1_mtx); snake1.set_direction(0); }
        if (IsKeyPressed(KEY_S)) { lock_guard<mutex> lk(snake1_mtx); snake1.set_direction(1); }
        if (IsKeyPressed(KEY_A)) { lock_guard<mutex> lk(snake1_mtx); snake1.set_direction(2); }
        if (IsKeyPressed(KEY_D)) { lock_guard<mutex> lk(snake1_mtx); snake1.set_direction(3); }

        if (IsKeyPressed(KEY_UP))    { lock_guard<mutex> lk(snake2_mtx); snake2.set_direction(0); }
        if (IsKeyPressed(KEY_DOWN))  { lock_guard<mutex> lk(snake2_mtx); snake2.set_direction(1); }
        if (IsKeyPressed(KEY_LEFT))  { lock_guard<mutex> lk(snake2_mtx); snake2.set_direction(2); }
        if (IsKeyPressed(KEY_RIGHT)) { lock_guard<mutex> lk(snake2_mtx); snake2.set_direction(3); }

        // Funções de DRAW
        BeginDrawing();
        ClearBackground(BLACK);

        // desenhar comidas
        {
            // Pega o acesso ao mutex das comidas e desenha cada uma na lista
            lock_guard<mutex> lk(foods_mtx);
            for (auto &f : foods) f.draw(CELL);
        }

        // desenhar cobrinhas
        {
            // Pega acesso ao mutex da cobrinha1 e desenha ela
            lock_guard<mutex> lk(snake1_mtx);
            snake1.draw(CELL);
        }
        {
            // Pega acesso ao mutex da cobrinha2 e desenha ela
            lock_guard<mutex> lk(snake2_mtx);
            snake2.draw(CELL);
        }

        // Desenhar placar dos players
        DrawRectangle(0, 0, 100, 60, Fade(DARKGRAY, 0.5f));
        DrawText(TextFormat("P1: %d | P2: %d", score1, score2), 10, 15, 30, WHITE);


        EndDrawing();
    }

    // Parte que fecha a janela
    running = false;
    if (spawner.joinable()) spawner.join();
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();

    CloseWindow();
    return 0;
}