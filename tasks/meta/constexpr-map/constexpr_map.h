#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        for (size_t i = 0; i < current_size_; ++i) {
            if (data_[i].first == key) {
                return data_[i].second;
            }
        }
        if (current_size_ == MaxSize) {
            throw std::runtime_error("size limit exceeded");
        }
        ++current_size_;
        data_[current_size_ - 1].first = key;
        return data_[current_size_ - 1].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (size_t i = 0; i < current_size_; ++i) {
            if (data_[i].first == key) {
                return data_[i].second;
            }
        }
        throw std::runtime_error("wrong key");
    }

    constexpr bool Erase(const K& key) {
        for (size_t i = 0; i < current_size_; ++i) {
            if (data_[i].first == key) {
                //                std::remove(data_[i], 1); // вот этот код не сработал, грустно
                for (size_t j = i + 1; j < current_size_; ++j) {
                    data_[j - 1] = data_[j];
                }
                --current_size_;
                return true;
            }
        }
        return false;
    }

    constexpr bool Find(const K& key) const {
        for (size_t i = 0; i < current_size_; ++i) {
            if (data_[i].first == key) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return current_size_;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        if (pos >= current_size_) {
            throw std::out_of_range("wrong pos");
        }
        return data_[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        if (pos >= current_size_) {
            throw std::out_of_range("wrong pos");
        }
        return data_[pos];
    }

protected:
    size_t current_size_ = 0;
    std::array<std::pair<K, V>, MaxSize> data_;
};
