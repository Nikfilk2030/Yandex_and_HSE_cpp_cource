#pragma once

#include <iostream>
#include <vector>
#include <memory>

struct State {
public:
    State() = default;
    State(size_t size) : vec_(std::vector<std::string>(size)) {
    }
    void operator()(const State& other) {
        ref_count = other.ref_count - 1;
        vec_ = other.vec_;
    }

public:
    size_t ref_count = 1;  // сколько векторов делят этот State между собой.
    std::vector<std::string> vec_;
};

class COWVector {
public:
    COWVector() {
        state_ = new State;
    }
    ~COWVector() {
        if (state_->ref_count == 0 || --state_->ref_count == 0) {
            delete state_;
        }
    }

    COWVector(const COWVector& other) {
        state_ = other.state_;
        ++state_->ref_count;
    }
    COWVector& operator=(const COWVector& other) {
        if (--state_->ref_count == 0) {
            delete state_;
        }
        state_ = other.state_;
        ++state_->ref_count;
        return *this;
    }

    size_t Size() const {
        return state_->vec_.size();
    }

    void Resize(size_t size) {
        if (--state_->ref_count == 0) {
            delete state_;
        }
        state_ = new State(size);  /// TODO, учесть размеры
    }

    const std::string& Get(size_t at) {
        return state_->vec_.at(at);  /// TODO
    }
    const std::string& Back() {
        return state_->vec_.back();
    }

    void PushBack(const std::string& value) {
        State* new_state = new State(*state_);
        if (--state_->ref_count == 0) {
            delete state_;
        }
        state_ = new_state;
        state_->ref_count = 1;

        state_->vec_.push_back(value);
    }

    void Set(size_t at, const std::string& value) {
        if (state_->vec_.at(at) == value) {
            return;
        }
        if (state_->ref_count == 1) {
            state_->vec_.at(at) = value;
            return;
        }
        State* new_state = new State(*state_);
        if (--state_->ref_count == 0) {
            delete state_;
        }
        state_ = new_state;
        --state_->ref_count;

        state_->vec_.at(at) = value;
    }

private:
    State* state_;
};
