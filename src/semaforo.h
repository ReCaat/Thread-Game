#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

using namespace std;

class Semaphore {
private:
    mutex m;
    condition_variable cv;
    int count;

public:
    explicit Semaphore(int init = 1) : count(init) {}

    // Wait é a função que requisita o semáforo e tenta acessá-lo.
    // Se o acesso for permitido (semáforo disponível), decrementa o contador
    // e pega o acesso do mutex.
    // Se o acesso for negado, espera até que seja liberado
    void wait() {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [&]{ return count > 0; });
        --count;
    }

    // Signal é a função que sinaliza para as threads esperando acesso
    // que o semáforo está disponível. Se estiver acessando, guarda o
    // mutex e incrementa o contador, liberando o acesso.
    // Se não, apenas incrementa o contador
    void signal() {
        {
            lock_guard<mutex> lk(m);
            ++count;
        }
        cv.notify_one();
    }
};

#endif
