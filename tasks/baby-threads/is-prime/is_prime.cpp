#include "is_prime.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

void JoinAll(std::vector<std::thread>& threads) {
    for (auto& thread : threads) {
        thread.join();
    }
}

bool IsPrimeSimple(uint64_t x) {
    if (x <= 1) {
        return false;
    }
    uint64_t root = sqrt(x);
    auto bound = std::min(root + 6, x);
    for (auto i = 2ull; i < bound; ++i) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
}

void OneThreadIsPrimeCheck(uint64_t x, uint64_t from, uint64_t to, std::atomic<bool>* global_ans) {
    if (*global_ans == false) {
        return;
    }
    int counter = 0;
    for (uint64_t i = from; i != to; ++i) {
        ++counter;
        if (counter % 100 == 0 && *global_ans == false) {  // подгоняемый параметр (после %)
            return;
        }
        if (x % i == 0) {
            *global_ans = false;
            return;
        }
    }
}

bool IsPrime(uint64_t x) {
    if (x <= 1) {
        return false;
    }

    static const uint64_t kMaxThreadCount = std::thread::hardware_concurrency();
    std::atomic<bool> number_is_prime = true;

    uint64_t root = sqrt(x);
    uint64_t step = root / kMaxThreadCount;
    if (step == 0) {
        step = 1;  // почему-то ругается на max
    }

    if (x < 1000) {
        return IsPrimeSimple(x);  // подгоняемый параметр
    }

    std::vector<std::thread> threads;
    for (uint64_t i = 2; i <= root; i += step) {
        if (!number_is_prime) {
            break;
        }
        threads.emplace_back(OneThreadIsPrimeCheck, x, i, i + step, &number_is_prime);
    }
    JoinAll(threads);
    return number_is_prime;
}
