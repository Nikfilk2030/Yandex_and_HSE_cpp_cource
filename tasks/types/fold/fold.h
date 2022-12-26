#pragma once

#include <vector>

struct Sum {
    int operator()(int a, int b) {
        return a + b;
    }
};

struct Prod {
    int operator()(int a, int b) {
        return a * b;
    }
};

struct Concat {
    std::vector<int> operator()(std::vector<int>& a, std::vector<int>& b) {
        for (const auto el : b) {
            a.emplace_back(el);
        }
        return a;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    for (Iterator it = first; it != last; ++it) {
        init = func(init, *it);
    }
    return init;
}

class Length {
public:
    Length(int* int_ptr) : int_ptr_(int_ptr){};

    template <typename T>
    T operator()(const T& a, const T& b) {
        (*int_ptr_) += 1;
        return a;
    }

protected:
    int* int_ptr_;
};

