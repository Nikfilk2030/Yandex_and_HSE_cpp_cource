#pragma once

#include <vector>

template <typename Iterator, typename Predicate, typename Functor>
void TransformIf(Iterator begin, Iterator end, Predicate predicate, Functor functor) {
    struct PreviousValue {
        typename std::iterator_traits<Iterator>::value_type prev_value;
        Iterator iterator;
    };
    std::vector<PreviousValue> previous_values;
    try {
        for (Iterator iterator = begin; iterator != end; ++iterator) {
            if (predicate(*iterator)) {
                try {
                    previous_values.push_back({*iterator, iterator});
                } catch (...) {
                };

                functor(*iterator);
            }
        }
    } catch (...) {
        for (auto& [previous_value, iterator] : previous_values) {
            try {
                *iterator = previous_value;
            } catch (...) {
                *iterator = previous_value;
            }
        }
        throw;
    }
}
