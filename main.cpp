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
constexpr int MAX_FOODS = 10;
constexpr int MAX_SPAWN_TRIES = 50;  // tentativas de spawnar evitando as cobras

// Recursos compartilhados
vector<Food> foods;
mutex foods_mtx;

Snake snake1(GRID_W/4, GRID_H/2, 0, 3);
Snake snake2(3*GRID_W/4, GRID_H/2, 1, 3);
mutex snake1_mtx;
mutex snake2_mtx;

// Semaforo compartilhado
Semaphore arena_sem(1);

atomic<bool> running(true);

// spawna comida evitando o corpo das cobras
void spawn_food_avoid_snakes() {
    for (int attempt=0; attempt < MAX_SPAWN_TRIES; ++attempt) {
        Food f(GRID_W, GRID_H);
        bool collide = false;

        // checa cobra 1
        {
            lock_guard<mutex> lk(snake1_mtx);
            for (auto &p : snake1.get_body()){
                if (p.first == f.getX() && p.second == f.getY()){
                    collide = true; 
                    break; 
                }
            }
        }
        if (collide) continue;

        // checa cobra 2
        {
            lock_guard<mutex> lk(snake2_mtx);
            for (auto &p : snake2.get_body()){
                if (p.first == f.getX() && p.second == f.getY()){
                    collide = true; 
                    break;
                }
            }
        }
        if (collide) continue;

        // Spawna comida
        {
            lock_guard<mutex> lk(foods_mtx);
            if ((int)foods.size() < MAX_FOODS) foods.push_back(f);
        }
        return;
    }
    // Se não tiver espaço, pula spawn
}

// thread para spawnar comida
void spawner_thread() {
    using namespace chrono;
    while (running) {
        this_thread::sleep_for(milliseconds(SPAWN_MS));

        spawn_food_avoid_snakes();
    }
}

// thread para cada tick de cada cobra
void snake_thread(Snake* s, mutex* s_mtx, Snake* e, mutex* e_mtx, int &enemy_score) {
    using namespace chrono;
    while (running) {
        this_thread::sleep_for(milliseconds(MOVE_MS));

        // tentativa de acessar arena
        arena_sem.wait();

        bool died = false;
        bool grew = false;

        // pega os locks dos mutexes
        {
            lock_guard<mutex> lk_sn(*s_mtx);
            lock_guard<mutex> lk_f(foods_mtx);
            lock_guard<mutex> lk_en(*e_mtx);
            auto nh = s->next_head();

            // verifica se bateu na parede
            if (nh.first < 0 || nh.first >= GRID_W || nh.second < 0 || nh.second >= GRID_H) {
                died = true;
            } 
            
            else {
                // verifica se comeu
                for (int i = 0; i < (int)foods.size(); ++i) {
                    if (foods[i].getX() == nh.first && foods[i].getY() == nh.second) {
                        foods.erase(foods.begin() + i);
                        grew = true;
                        break;
                    }
                }
                // movimento
                s->apply_move(grew);

                // verifica se bate em si mesmo
                if (s->check_self_collision()){
                    died = true;
                }

                if(s->check_enemy_collision(*e)){
                    died = true;
                }
            }

            if (died) {
                if (s == &snake1)
                    s->reset(GRID_W/4, GRID_H/2, 3);
                else
                    s->reset(3*GRID_W/4, GRID_H/2, 3);

                enemy_score += 1;

                foods.clear();
            }
        }

        arena_sem.signal();
    }
}

int main() {
    InitWindow(GRID_W * CELL, GRID_H * CELL, "Snake Multiplayer Threads");
    SetTargetFPS(60);

    // Spawn de comida inicial
    {
        lock_guard<mutex> lk(foods_mtx);
        foods.push_back(Food(GRID_W, GRID_H));
    }

    // placar
    int score1 = 0;
    int score2 = 0;

    // threads
    thread spawner(spawner_thread);
    thread t1(snake_thread, &snake1, &snake1_mtx, &snake2, &snake2_mtx, ref(score2));
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
            lock_guard<mutex> lk(foods_mtx);
            for (auto &f : foods) f.draw(CELL);
        }

        // desenhar cobrinhas
        {
            lock_guard<mutex> lk(snake1_mtx);
            snake1.draw(CELL);
        }
        {
            lock_guard<mutex> lk(snake2_mtx);
            snake2.draw(CELL);
        }

        // Desenhar placar
        DrawRectangle(0, 0, 100, 60, Fade(DARKGRAY, 0.5f));
        DrawText(TextFormat("P1: %d | P2: %d", score1, score2), 10, 15, 30, WHITE);


        EndDrawing();
    }

    // Processo de fechamento
    running = false;
    if (spawner.joinable()) spawner.join();
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();

    CloseWindow();
    return 0;
}