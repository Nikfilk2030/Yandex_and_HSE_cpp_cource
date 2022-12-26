#pragma once

#include <chrono>
#include <condition_variable>
#include <list>
#include <map>
#include <mutex>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

public:
    void Add(const T& item, TimePoint at) {
        std::unique_lock<std::mutex> lock(mutex_);
        data_.insert({at, item});
        cond_var_.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (data_.empty()) {
            cond_var_.wait(lock);
        }
        auto time_and_value = *data_.begin();
        TimePoint end_time = time_and_value.first;
        T value = time_and_value.second;

        data_.erase(data_.begin());
        cond_var_.wait_until(lock, end_time);
        return value;
    }

protected:
    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::multimap<TimePoint, T> data_;
};
