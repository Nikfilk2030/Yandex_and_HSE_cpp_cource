#pragma once

#include <condition_variable>
#include <list>
#include <mutex>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count){};

    void Leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cond_vars_.empty()) {
            ++count_;
            cond_vars_.front()->notify_one();
            cond_vars_.pop_front();
        }
    }

    template <class Func>
    void Enter(Func callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ == 0) {  /// может как-то вообще без count_ делать?
            cond_vars_.emplace_back(std::make_shared<std::condition_variable>());
            cond_vars_.back()->wait(lock);
        }
        callback(--count_);
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::mutex mutex_;
    int count_ = 0;
    std::list<std::shared_ptr<std::condition_variable>> cond_vars_;
};
