#pragma once

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>
#include <vector>

void LockAll(std::vector<std::mutex>& mutexes) {
    for (auto& mutex : mutexes) {
        mutex.lock();
    }
};

void UnlockAll(std::vector<std::mutex>& mutexes) {
    for (auto& mutex : mutexes) {
        mutex.unlock();
    }
};

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
    using Bucket = std::vector<std::pair<K, V>>;

    size_t GetBucketNum(size_t hash, const K& key) const {
        return hash % table_.size();
    }  /// я хотел это использовать, но race

    size_t GetMutexNum(size_t hash, const K& key) const {
        return hash % mutexes_.size();
    }  /// я хотел это использовать, но race

public:
    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher){};

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher){};

    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash())
        : hash_(hasher), table_(kDefaultMutexCount), mutexes_(kDefaultMutexCount) {
        (void)expected_threads_count;
    }

    void Copy(const std::vector<Bucket>& table, std::vector<Bucket>* new_table) {
        for (auto& bucket : table_) {
            for (auto& [current_key, current_value] : bucket) {
                size_t new_bucket_num = hash_(current_key) % new_table->size();
                (*new_table)[new_bucket_num].emplace_back(current_key, current_value);
            }
        }
    }

    void Rehash(size_t bucket_index) {
        if (table_[bucket_index].size() <= kMaxBucketSize) {  // Тагир рассказывал об этой проблеме
            return;
        }

        std::vector<Bucket> new_table(table_.size() * kTableGrowthCoefficient);
        Copy(table_, &new_table);
        table_ = std::move(new_table);
    }

    bool Insert(const K& key, const V& value) {
        std::unique_lock<std::mutex> lock(mutexes_[hash_(key) % mutexes_.size()]);

        size_t bucket_num = hash_(key) % table_.size();
        for (auto& [current_key, current_value] : table_[bucket_num]) {
            if (current_key != key) {
                continue;
            }
            return false;  // у нас уже был этот элемент
        }
        table_[bucket_num].push_back({key, value});
        ++size_;
        if (table_[bucket_num].size() <= kMaxBucketSize) {
            return true;
        }
        // а вот тут надо делать rehash!
        lock.unlock();
        LockAll(mutexes_);
        Rehash(bucket_num);
        UnlockAll(mutexes_);
        return true;
    }

    bool Erase(const K& key) {
        std::unique_lock<std::mutex> lock(mutexes_[hash_(key) % mutexes_.size()]);

        size_t bucket_num = hash_(key) % table_.size();
        for (auto it = table_[bucket_num].begin(); it != table_[bucket_num].end(); ++it) {
            if (it->first != key) {
                continue;
            }
            table_[bucket_num].erase(it);
            --size_;
            return true;
        }
        return false;
    }

    std::pair<bool, V> Find(const K& key) const {
        std::unique_lock<std::mutex> lock(mutexes_[hash_(key) % mutexes_.size()]);

        size_t bucket_num = hash_(key) % table_.size();
        for (auto& [current_key, current_value] : table_[bucket_num]) {
            if (current_key != key) {
                continue;
            }
            return std::make_pair(true, current_value);
        }
        return std::make_pair(false, V());  // TODO попробовать без V(), или V, или понять
    }

    const V At(const K& key) const {
        std::unique_lock<std::mutex> lock(mutexes_[hash_(key) % mutexes_.size()]);

        size_t bucket_num = hash_(key) % table_.size();
        for (auto& [current_key, current_value] : table_[bucket_num]) {
            if (current_key == key) {
                return current_value;
            }
        }
        throw std::out_of_range("AT returned false");
    }

    void Clear() {
        LockAll(mutexes_);
        for (Bucket& bucket : table_) {
            bucket.clear();
        }
        size_ = 0;
        UnlockAll(mutexes_);
    }

    size_t Size() const {
        return size_;
    }

    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;
    static const int kMaxBucketSize = 77;      /// TODO поменять
    static const int kDefaultMutexCount = 20;  /// TODO поменять
    static const int kTableGrowthCoefficient = 3;

private:
    Hash hash_;
    std::vector<Bucket> table_;
    std::atomic<size_t> size_ = 0;
    mutable std::vector<std::mutex> mutexes_;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = -1;

