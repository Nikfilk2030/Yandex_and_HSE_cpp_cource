#pragma once

#include <string>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
public:
    virtual ~OneTimeCallback() {
    }
    virtual std::string operator()() const&& = 0;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback(const std::string other_str) : str_(other_str){};

    std::string operator()() const&& override {
        std::string tmp_str = str_ + "awesomeness";
        delete this;
        return tmp_str;
    }

private:
    std::string str_;
};
