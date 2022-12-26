#include "lru_cache.h"

LruCache::LruCache(size_t max_size) : max_size_(max_size) {
}

// Для реализации нам понадобится list, который будет являться очередью, и map, которая
// будет следить за позициями в list
void LruCache::Set(const std::string& key, const std::string& value) {
    auto it = container_.find(key);
    if (it != container_.end()) {
        MoveToFirst(key);
        it->second = value;
        return;
    }
    ++cache_size_;

    priorities_.push_front(key);
    str_to_iter_[key] = priorities_.begin();

    container_[key] = value;

    if (cache_size_ > max_size_) {
        --cache_size_;
        auto to_delete = priorities_.back();
        priorities_.erase(--priorities_.end());
        container_.erase(to_delete);
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    auto it = container_.find(key);
    if (it == container_.end()) {
        return false;
    } else {
        *value = container_[key];
        MoveToFirst(key);
        return true;
    }
}

void LruCache::MoveToFirst(std::string key) {
    priorities_.erase(str_to_iter_[key]);
    priorities_.push_front(key);
    str_to_iter_[key] = priorities_.begin();
}
