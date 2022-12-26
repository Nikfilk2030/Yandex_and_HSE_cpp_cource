#pragma once

#include <math.h>

#include <string>
#include <string_view>
#include <vector>
#include <cstring>

// — проверяет, что строка `string` начинается с `text`.
bool StartsWith(std::string_view string, std::string_view text);

// проверяет, что строка `string` оканчивается на `text`
bool EndsWith(std::string_view string, std::string_view text);

// возвращает `string` с убранным `prefix`,
// если `string` не начинается на `prefix`, возвращает `string`.
std::string_view StripPrefix(std::string_view string, std::string_view prefix);

// тоже самое, но с суффиксом.
std::string_view StripSuffix(std::string_view string, std::string_view suffix);

// — тоже самое, что и `s.substr(pos, n)`, но если `n` больше `s.size()`, то возвращается `s`.
std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n = std::string_view::npos);

// — `strip` строки, удаляем все символы с обоих концов вида isspace
std::string_view StripAsciiWhitespace(std::string_view string);

// — добавляет к `path` файловой системы символ `/`, если его не было.
std::string AddSlash(std::string_view path);

// — убирает `/` из `path`, если это не сам путь `/` и путь заканчивается на `/`.
std::string_view RemoveSlash(std::string_view path);

//  — известно, что `path` — корректный путь до файла, верните его название.
std::string_view Basename(std::string_view path);

// — известно, что `path` — корректный путь до файла без слеша на конце,
// верните папку, в которой этот файл лежит без слеша на конце, если это не корень.
std::string_view Dirname(std::string_view path);

// — известно, что `path` — корректный путь, но `/` могут повторяться, надо убрать все повторения.
std::string CollapseSlashes(std::string_view path);

// — склеить все строки в одну через `delimiter`. Обойдитесь одной аллокацией памяти.
std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);

// — открывает файл и читает `n` байт из filename. Используйте Linux Syscalls
//  `open`, `read`, `close`. Если открыть или прочитать файл нельзя, возвращает пустую строчку.
std::string ReadN(const std::string& filename, size_t n);

// — делаем `split` строки по `delim`. Подумайте,
// прежде чем копипастить из уже имеющейся задачи. Обойдитесь одной аллокацией памяти.
std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);

template <typename NumType>
inline void Size(size_t& total_size, NumType num) {
    if (num == 0) {
        total_size += 1;
        return;
    }
    if (num < 0) {
        total_size += 1;
    }
    size_t size = 0;
    while (num != 0) {
        ++size;
        num /= 10;
    }
    total_size += size;
}

inline void Size(size_t& total_size, const std::string& str) {
    total_size += str.size();
}

inline void Size(size_t& total_size, std::string_view str) {
    total_size += str.size();
}

inline void Size(size_t& total_size, const char* str) {
    total_size += std::strlen(str);
}

inline void TotalSize(size_t& total_size){};

template <typename ObjType, typename... Args>
inline void TotalSize(size_t& total_size, ObjType&& current_obj, Args&&... others) {
    Size(total_size, current_obj);
    TotalSize(total_size, std::forward<Args>(others)...);
}

template <typename NumType>
inline void PushBack(std::string& answer, NumType const_element) {
    if (const_element == 0) {
        answer += '0';
        return;
    }
    NumType current_element = const_element;
    if (const_element < 0) {
        answer += '-';
    }
    size_t element_size = 0;
    while (current_element != 0) {
        ++element_size;
        current_element /= 10;
    }
    for (size_t i = 0; i < element_size; ++i) {
        answer += '!';  // get away from it
    }
    size_t pos = answer.size() - 1;
    current_element = const_element;
    while (current_element != 0) {
        int i_dont_know_why_i_cant_use_abs_here = current_element % 10;
        if (i_dont_know_why_i_cant_use_abs_here < 0) {
            i_dont_know_why_i_cant_use_abs_here = -i_dont_know_why_i_cant_use_abs_here;
        }
        answer[pos--] = i_dont_know_why_i_cant_use_abs_here + '0';
        current_element /= 10;
    }
}

inline void PushBack(std::string& answer, const char* str) {
    answer += str;
}

inline void PushBack(std::string& answer, std::string_view str) {
    answer += str;
}

inline void PushBack(std::string& answer, std::string& str) {  // std::string&
    answer += str;
}

inline void TotalPushBack(std::string& answer){};

template <typename ObjType, typename... Args>
inline void TotalPushBack(std::string& answer, ObjType&& current_obj, Args&&... others) {
    PushBack(answer, std::forward<ObjType>(current_obj));
    TotalPushBack(answer, std::forward<Args>(others)...);
}

// — склеить все аргументы в один в их строковом представлении.
//    Должны поддерживаться числа (`int, long, long long` и их `unsigned` версии), также все
//    строковые типы (`std::string, std::string_view, const char*`). Аргументов в `StrCat` не больше
//    пяти. Придумайте как это сделать за одну аллокацию памяти.
template <typename... Args>
std::string StrCat(Args&&... args) {
    size_t total_size = 0;
    TotalSize(total_size, std::forward<Args>(args)...);

    std::string answer;
    answer.reserve(total_size);

    TotalPushBack(answer, std::forward<Args>(args)...);

    return answer;
}
