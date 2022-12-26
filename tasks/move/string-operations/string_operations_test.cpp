#include "string_operations.h"

#include <fcntl.h>
#include <unistd.h>

#include <fstream>

bool StartsWith(std::string_view string, std::string_view text) {
    return string.starts_with(text);
}

bool EndsWith(std::string_view string, std::string_view text) {
    return string.ends_with(text);
}

std::string_view StripPrefix(std::string_view string, std::string_view prefix) {
    if (string.starts_with(prefix)) {
        return string.substr(prefix.size(), string.size());
    } else {
        return string;
    }
}

std::string_view StripSuffix(std::string_view string, std::string_view suffix) {
    if (string.ends_with(suffix)) {
        return string.substr(0, string.size() - suffix.size());
    } else {
        return string;
    }
}

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n) {
    if (n > s.size()) {
        return s;
    }
    return s.substr(pos, n);
}

std::string_view StripAsciiWhitespace(std::string_view string) {
    if (string.empty()) {
        return string;
    }
    size_t start_pos = 0;
    size_t end_pos = string.size() - 1;
    while (isspace(string[start_pos]) && start_pos < string.size()) {
        ++start_pos;
    }
    while (end_pos > start_pos && isspace(string[end_pos])) {
        --end_pos;
    }
    return string.substr(start_pos, end_pos - start_pos + 1);
}

std::string StringViewToString(std::string_view path, size_t size) {
    std::string string(size, '0');
    for (size_t i = 0; i != path.size(); ++i) {
        string[i] = path[i];
    }
    return string;
}

std::string AddSlash(std::string_view path) {
    if (path.empty() || path.back() != '/') {
        std::string string = StringViewToString(path, path.size() + 1);
        string.back() = '/';
        return string;
    }
    return StringViewToString(path, path.size());
}

std::string_view RemoveSlash(std::string_view path) {
    if (path.size() <= 1) {
        return path;
    }
    if (path.back() == '/') {
        path = path.substr(0, path.size() - 1);
    }
    return path;
}

std::string_view Basename(std::string_view path) {
    size_t start_pos = path.size() - 1;
    while (path[start_pos] != '/') {
        --start_pos;
    }
    return path.substr(start_pos + 1);
}

std::string_view Dirname(std::string_view path) {
    size_t start_pos = path.size() - 1;
    while (path[start_pos] != '/') {
        --start_pos;
    }
    if (start_pos == 0) {
        return path.substr(0, 1);
    }
    return path.substr(0, start_pos);
}

std::string CollapseSlashes(std::string_view path) {
    size_t repeated_slash_count = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i] == path[i - 1] && path[i] == '/') {
            ++repeated_slash_count;
        }
    }
    std::string string(path.size() - repeated_slash_count, '0');
    size_t string_ind = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0 && path[i] == path[i - 1] && path[i] == '/') {
            continue;
        }
        string[string_ind++] = path[i];
    }
    return string;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return "";
    }
    size_t strings_size_sum = 0;
    for (auto string : strings) {
        strings_size_sum += string.size();
    }
    std::string answer;
    answer.reserve(strings_size_sum + delimiter.size() * (strings.size() - 1));
    for (size_t i = 0; i < strings.size() - 1; ++i) {
        answer += strings[i];
        answer += delimiter;
    }
    answer += strings.back();
    return answer;
}

std::string ReadN(const std::string& filename, size_t n) {
    try {
        std::string answer(n, '0');
        int file = open(&filename[0], std::ios::binary);
        read(file, &answer[0], n);
        close(file);
        return answer;
    } catch (...) {
        return "";
    }
}

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    std::vector<std::string_view> splitted_strings;
    std::string_view string = text;
    size_t delim_position = string.find(delim);
    size_t string_count = 0;

    while (delim_position != std::string::npos) {
        ++string_count;
        string = string.substr(delim_position + delim.size());
        delim_position = string.find(delim);
    }

    splitted_strings.resize(string_count + 1);

    string = text;
    size_t prev_position = 0;

    for (size_t i = 0; i != string_count; ++i) {
        size_t word_end_ind = string.find(delim, prev_position);
        splitted_strings[i] = (string.substr(prev_position, word_end_ind - prev_position));
        prev_position = word_end_ind + delim.size();
    }
    if (prev_position != text.size()) {
        splitted_strings.back() = string.substr(prev_position);
    }

    return splitted_strings;
}