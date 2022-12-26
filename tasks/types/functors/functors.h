#pragma once

#include <vector>

template <class Functor>
class ReverseBinaryFunctor {
public:
    ReverseBinaryFunctor(Functor func) : func_(func){};

    template <typename T1, typename T2>
    bool operator()(T1 a, T2 b) {
        return !func_(a, b);
    }

protected:
    Functor func_;
};

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}

template <class Functor>
class ReverseUnaryFunctor {
public:
    ReverseUnaryFunctor(Functor func) : func_(func){};

    template <typename T>
    bool operator()(T a) {
        return !func_(a);
    }

protected:
    Functor func_;
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}
