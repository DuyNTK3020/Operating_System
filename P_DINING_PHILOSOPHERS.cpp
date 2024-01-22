#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

const int num_philosophers = 5;

mutex fork_mutex[num_philosophers];
condition_variable fork_cv[num_philosophers];
bool fork_available[num_philosophers];

void philosopher(int id) {
    int left_fork = id;
    int right_fork = (id + 1) % num_philosophers;

    while (true) {
        // Thinking
        cout << "Philosopher " << id << " is thinking.\n";

        // Pick up left fork
        unique_lock<mutex> left_lock(fork_mutex[left_fork]);
        while (!fork_available[left_fork]) {
            fork_cv[left_fork].wait(left_lock);
        }
        fork_available[left_fork] = false;

        // Pick up right fork
        unique_lock<mutex> right_lock(fork_mutex[right_fork]);
        while (!fork_available[right_fork]) {
            fork_cv[right_fork].wait(right_lock);
        }
        fork_available[right_fork] = false;

        // Eating
        cout << "Philosopher " << id << " is eating.\n";

        // Put down left fork
        fork_available[left_fork] = true;
        left_lock.unlock();
        fork_cv[left_fork].notify_one();

        // Put down right fork
        fork_available[right_fork] = true;
        right_lock.unlock();
        fork_cv[right_fork].notify_one();
    }
}

int main() {
    thread philosophers[num_philosophers];

    for (int i = 0; i < num_philosophers; ++i) {
        philosophers[i] = thread(philosopher, i);
    }

    for (int i = 0; i < num_philosophers; ++i) {
        philosophers[i].join();
    }

    return 0;
}
