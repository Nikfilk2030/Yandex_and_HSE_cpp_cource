#pragma once

#include <list>
#include <string>
#include <unordered_map>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

    void MoveToFirst(std::string key);

protected:
    size_t max_size_ = 0;
    size_t cache_size_ = 0;
    std::list<std::string> priorities_;
    std::unordered_map<std::string, std::string> container_;
    std::unordered_map<std::string, std::list<std::string>::iterator> str_to_iter_;
};
