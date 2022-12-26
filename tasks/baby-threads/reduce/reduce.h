#pragma once
#include <deque>
#include <thread>
#include <vector>

static const uint64_t kMaxThreadCount = std::thread::hardware_concurrency();

void JoinAll(std::vector<std::thread>& threads) {
    for (auto& thread : threads) {
        thread.join();
    }
}

template <class RandomAccessIterator, class T, class Func>
T ReduceNoThreads(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value,
                  Func func) {
    auto cur_value(initial_value);
    while (first != last) {
        cur_value = func(cur_value, *first++);
    }
    return cur_value;
}

template <class RandomAccessIterator, class T, class Func>
void ReduceOneThread(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value,
                     Func func, std::deque<T>* thread_reduce_values_ptr, uint64_t i) {
    T this_thread_reduce_result = ReduceNoThreads(first, last, initial_value, func);
    (*thread_reduce_values_ptr)[i] = this_thread_reduce_result;
}

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    uint64_t distance = std::distance(first, last);

    std::vector<std::thread> threads;
    threads.reserve(kMaxThreadCount);

    std::deque<T> thread_reduce_values(kMaxThreadCount);

    uint64_t step = distance / kMaxThreadCount;
    for (size_t i = 0; i < kMaxThreadCount - 1; ++i) {
        threads.emplace_back(ReduceOneThread<RandomAccessIterator, T, Func>,
                             first + i * step,        // NOLINT
                             first + (i + 1) * step,  // NOLINT
                             initial_value, func,     // NOLINT
                             &thread_reduce_values, i);
    }
    // последний мёрджим до last
    threads.emplace_back(ReduceOneThread<RandomAccessIterator, T, Func>,
                         first + (kMaxThreadCount - 1) * step,  // NOLINT
                         last,                                  // NOLINT
                         initial_value, func,                   // NOLINT
                         &thread_reduce_values, kMaxThreadCount - 1);

    JoinAll(threads);

    return ReduceNoThreads(thread_reduce_values.begin(),  // NOLINT
                           thread_reduce_values.end(),    // NOLINT
                           initial_value, func);
}
