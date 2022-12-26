#pragma once

#include <cstring>
#include <memory>
#include <string>

class StringView {
public:
    StringView(const char* str) : str_(str), size_(strlen(str)){};

    StringView(const char* str, size_t size) : str_(str), size_(size){};

    StringView(const std::string& str, size_t start = 0, size_t size = std::string::npos) {
        str_ = &str[start];
        if (size <= str.size()) {
            size_ = size;
        } else {
            size_ = str.size() - start;
        }
    }

    char operator[](int i) const {
        return *(str_ + i);
    }

    size_t Size() const {
        return size_;
    }

protected:
    const char* str_ = nullptr;
    size_t size_ = std::string::npos;
};