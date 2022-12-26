#pragma once

#include <condition_variable>
#include <mutex>

class RWLock {
public:
    template <class Func>
    void Read(Func func) {
        mutex_.lock();
        ++blocked_readers_;
        mutex_.unlock();
        try {
            func();
        } catch (...) {
            EndRead();
            throw;
        }
        EndRead();
    }

    template <class Func>
    void Write(Func func) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (blocked_readers_ != 0) {
            writers_.wait(lock);
        }
        func();
        writers_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable writers_;
    int blocked_readers_ = 0;

    void EndRead() {
        mutex_.lock();
        --blocked_readers_;
        if (blocked_readers_ == 0) {
            writers_.notify_one();
        }
        mutex_.unlock();
    }
};
