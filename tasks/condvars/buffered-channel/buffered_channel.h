#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <optional>
#include <utility>

// я не понимаю, почему не заходит

template <class T>
class BufferedChannel {

#define CheckIsClosedForSend(is_closed_, wait_elements_)           \
    if (is_closed_) {                                              \
        waiters_.notify_all();                                     \
        throw std::runtime_error("Cannot send in closed channel"); \
    }

public:
    explicit BufferedChannel(int size) : max_size_(size){};

    void Send(const T& value) {
        std::unique_lock lock(mutex_);
        CheckIsClosedForSend(is_closed_, waiters_);
        while (data_.size() > max_size_) {
            CheckIsClosedForSend(is_closed_, waiters_);
            writers_.wait(lock);
            if (data_.size() == max_size_) {
                CheckIsClosedForSend(is_closed_, waiters_);
            }
        }
        CheckIsClosedForSend(is_closed_, waiters_);
        data_.push_back(value);
        waiters_.notify_one();
    }

    std::optional<T> Recv() {
        std::unique_lock lock(mutex_);
        while (data_.empty()) {
            if (is_closed_) {
                waiters_.notify_all();
                return std::nullopt;
            }
            waiters_.wait(lock);
        }
        T value = data_.front();
        data_.pop_front();
        writers_.notify_one();
        return value;
    }

    void Close() {
        std::unique_lock lock(mutex_);
        is_closed_ = true;
        waiters_.notify_all();
        writers_.notify_all();
    }

protected:
    int max_size_ = 0;
    std::mutex mutex_;
    std::list<T> data_;
    bool is_closed_ = false;
    std::condition_variable waiters_;
    std::condition_variable writers_;
};
