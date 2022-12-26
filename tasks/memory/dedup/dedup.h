#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> duplicated;
    duplicated.reserve(items.size());
    for (auto item_ptr : items) {
        duplicated.push_back(std::make_unique<string>(*item_ptr));
    }
    return duplicated;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::map<string, std::shared_ptr<string>> string_to_ptr;
    std::vector<std::shared_ptr<string>> deduplicated;
    deduplicated.reserve(items.size());
    for (auto& item_ptr : items) {
        auto has_string = string_to_ptr.find(*item_ptr);
        if (has_string == string_to_ptr.end()) {
            string_to_ptr[*item_ptr] = std::make_shared<string>(*item_ptr);
        }
        deduplicated.push_back(string_to_ptr[*item_ptr]);
    }
    return deduplicated;
}
