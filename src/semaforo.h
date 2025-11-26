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

    void wait() {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [&]{ return count > 0; });
        --count;
    }

    void signal() {
        {
            lock_guard<mutex> lk(m);
            ++count;
        }
        cv.notify_one();
    }
};

#endif
