#pragma once

#include <iostream>

template <class Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end){};

    Iterator begin() const {  // NOLINT
        return begin_;
    }

    Iterator end() const {  // NOLINT
        return end_;
    }

private:
    Iterator begin_;
    Iterator end_;
};

template <typename T>
class RangeIt {
public:
    RangeIt(T begin, T step = 1) : current_it_(begin), step_(step){};

    bool operator!=(const RangeIt& other) {
        return current_it_ < other.current_it_;
    }

    bool operator==(const RangeIt& other) {
        return current_it_ >= other.current_it_;
    }

    RangeIt& operator++() {
        current_it_ += step_;
        return *this;
    }

    T& operator*() {
        return current_it_;
    }

protected:
    T current_it_;
    T step_;
};

template <typename T>
auto Range(const T from, const T to, const T step = 1) {
    return IteratorRange(RangeIt(from, step), RangeIt(to));
}

template <typename T>
auto Range(const T to) {
    T from = 0;
    return IteratorRange(RangeIt(from), RangeIt(to));
}

template <typename FirstIt, typename SecondIt>
class ZipIt {
public:
    ZipIt(const FirstIt first_it, const SecondIt second_it)
        : first_it_(first_it), second_it_(second_it){};

    bool operator!=(const ZipIt& other) {
        return first_it_ != other.first_it_ && second_it_ != other.second_it_;  // TODO || matters
    }

    ZipIt& operator++() {
        ++first_it_;
        ++second_it_;
        return *this;
    }

    auto operator*() {
        return std::make_pair(*first_it_, *second_it_);
    }

protected:
    FirstIt first_it_;
    SecondIt second_it_;
};

template <typename Sequence1, typename Sequence2>
auto Zip(const Sequence1& first, const Sequence2& second) {
    return IteratorRange(ZipIt(std::begin(first), std::begin(second)),
                         ZipIt(std::end(first), std::end(second)));
}

template <typename SequenceType>
class GroupIt {
    using SequenceIt = decltype(std::begin(std::declval<SequenceType>()));

public:
    GroupIt(const SequenceType& sequence)
        : start_(std::begin(sequence)), end_(std::end(sequence)){};

    class GroupElemIt {
    public:
        GroupElemIt(SequenceIt begin, SequenceIt end) : current_it_(begin), end_it_(end){};

        bool operator!=(GroupElemIt other) {
            return current_it_ != other.current_it_;
        }

        GroupElemIt& operator++() {
            SequenceIt curr_it = current_it_;
            ++current_it_;
            while (current_it_ != end_it_) {
                if (*current_it_ != *curr_it) {
                    break;
                }
                ++current_it_;
            }
            return *this;
        }

        SequenceType operator*() {
            SequenceIt curr_it = current_it_;
            ++curr_it;
            while (curr_it != end_it_) {
                if (*current_it_ != *curr_it) {
                    break;
                }
                ++curr_it;
            }
            return SequenceType(current_it_, curr_it);
        }

        SequenceIt current_it_;
        SequenceIt end_it_;
    };

    GroupElemIt begin() const {  // NOLINT
        return GroupElemIt(start_, end_);
    }

    GroupElemIt end() const {  // NOLINT
        return GroupElemIt(end_, end_);
    }

protected:
    SequenceIt start_;
    SequenceIt end_;
};

template <typename SequenceType>
auto Group(const SequenceType& sequence) {
    return GroupIt(sequence);
}
