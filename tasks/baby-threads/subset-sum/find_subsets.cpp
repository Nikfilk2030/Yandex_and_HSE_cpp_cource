#include "find_subsets.h"

#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

std::mutex mutex;
std::atomic<bool> flag = false;
std::string left_answer;
std::string right_answer;

uint8_t right_start;
uint8_t right_end;
uint8_t left_end;

bool Contains(const std::string& line, char target) {
    return line.find(target) != std::string::npos;
}

#define Check(flag) \
    if (flag) {     \
        return;     \
    }

void CalculateRight(std::string right_mask,
                    std::unordered_map<int64_t, std::string>* left_val_to_str, int64_t curr_res,
                    const std::vector<int64_t>* data) {
    if (right_mask.length() >= right_end) {
        Check(flag);
        auto it = left_val_to_str->find(curr_res);
        if (it == left_val_to_str->end()) {
            return;
        }
        std::string left_mask = it->second;
        if ((!Contains(right_mask, '1') && !Contains(left_mask, '1')) ||
            (!Contains(right_mask, '2') && !Contains(left_mask, '2'))) {
            return;
        }

        mutex.lock();
        if (!flag) {
            right_answer = right_mask;
            left_answer = left_mask;
            flag = true;
        }
        mutex.unlock();
        return;
    }

    /// рекурсивно вызываем:

    int64_t current_res_addition = data->at(right_mask.size() + right_start);

    Check(flag);
    right_mask.push_back('0');
    CalculateRight(right_mask, left_val_to_str, curr_res, data);
    right_mask.pop_back();

    Check(flag);
    right_mask.push_back('1');
    CalculateRight(right_mask, left_val_to_str, curr_res - current_res_addition, data);
    right_mask.pop_back();

    Check(flag);
    right_mask.push_back('2');
    CalculateRight(right_mask, left_val_to_str, curr_res + current_res_addition, data);
    right_mask.pop_back();
}

void CalculateLeft(std::string left_masks,
                   std::unordered_map<int64_t, std::string>* left_val_to_str,
                   const std::vector<int64_t>* data, int8_t size = 0) {
    if (size >= left_end) {  /// >=
        int64_t result = 0;
        for (int8_t i = 0; i < left_end; ++i) {
            if (left_masks[i] == '0') {
                continue;
            } else if (left_masks[i] == '1') {
                result += data->at(i);
            } else {
                result -= data->at(i);
            }
        }
        (*left_val_to_str)[result] = left_masks;
        return;
    }

    for (uint8_t i = 0; i < 3; ++i) {
        left_masks[size] = '0' + i;
        CalculateLeft(left_masks, left_val_to_str, data, size + 1);
    }
}

void JoinAll(std::vector<std::thread>& threads) {
    for (auto& thread : threads) {
        thread.join();
    }
}

Subsets FindEqualSumSubsets(const std::vector<int64_t>& data) {
    if (data.size() <= 1) {
        return Subsets{.exists = false};
    }
    if (data.size() == 2) {
        if (data[0] == data[1]) {
            return Subsets{.first_indices = {0}, .second_indices = {1}, .exists = true};
        }
        return Subsets{.exists = false};
    }

    flag = false;
    left_answer.clear();
    right_answer.clear();

    uint8_t left_size = data.size() / 2 - 1;
    uint8_t right_size = data.size() - left_size;

    left_answer.reserve(left_size);
    right_answer.reserve(right_size);

    right_start = left_size;
    left_end = left_size;
    right_end = data.size() - left_end;

    /// подготовка закончена, дальше левая часть:

    std::string right_mask;
    std::unordered_map<int64_t, std::string> left_val_to_str;
    std::string left_masks(left_size, '0');

    CalculateLeft(left_masks, &left_val_to_str, &data, 0);

    /// теперь правая (многопоточная) часть

    std::vector<std::thread> threads;
    std::map<uint8_t, int64_t> i_to_start_res = {{0, 0},                   // NOLINT
                                                 {1, -data[right_start]},  // NOLINT
                                                 {2, data[right_start]}};

    for (uint8_t i = 0; i < 3; ++i) {
        int64_t current_result = i_to_start_res[i];
        std::string str;
        str.reserve(right_size);
        str.push_back('0' + i);
        threads.emplace_back(CalculateRight,    // NOLINT
                             str,               // NOLINT
                             &left_val_to_str,  // NOLINT
                             current_result,    // NOLINT
                             &data);
    }
    JoinAll(threads);

    /// теперь собираем ответ

    if (!flag) {
        return Subsets{.exists = false};
    }
    Subsets answer{.exists = true};
    for (uint8_t i = 0; i < left_size; ++i) {
        if (left_answer[i] == '0') {
            continue;
        }
        if (left_answer[i] == '1') {
            answer.first_indices.push_back(i);
        } else {
            answer.second_indices.push_back(i);
        }
    }
    for (uint8_t i = 0; i < right_size; ++i) {
        if (right_answer[i] == '0') {
            continue;
        }
        if (right_answer[i] == '1') {
            answer.first_indices.push_back(i + left_size);
        } else {
            answer.second_indices.push_back(i + left_size);
        }
    }
    return answer;
}
