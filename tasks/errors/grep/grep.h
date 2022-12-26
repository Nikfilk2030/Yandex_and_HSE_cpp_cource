#pragma once

#include <cstring>
#include <exception>
#include <memory>
#include <stdexcept>

template <class T>
class Try {
public:
    Try() = default;

    explicit Try(const T& value) : value_ptr_(std::make_unique<T>(value)){};

    template <typename Exception>
    Try(const Exception& exception) : exception_ptr_(std::make_exception_ptr(exception)){};

public:
    void CheckExceptionPtr() {
        if (exception_ptr_ != nullptr) {
            std::rethrow_exception(exception_ptr_);
        }
    }

    Try(const std::exception_ptr& exception_ptr) : exception_ptr_(exception_ptr){};

    bool IsFailed() {
        return exception_ptr_ != nullptr;
    }

    void CheckElementPtr() {
        if (value_ptr_ == nullptr) {
            throw std::logic_error("Object is empty");
        }
    }

    T& Value() {
        CheckExceptionPtr();
        CheckElementPtr();
        return *value_ptr_;
    }

    void Throw() {
        CheckExceptionPtr();
        throw std::logic_error("No exception");
    }

protected:
    std::exception_ptr exception_ptr_ = nullptr;
    std::unique_ptr<T> value_ptr_ = nullptr;
};

template <>
class Try<void> {
public:
    Try() = default;

    template <typename Exception>
    Try(const Exception& exception) : exception_ptr_(std::make_exception_ptr(exception)){};

    Try(const std::exception_ptr& exception_ptr) : exception_ptr_(exception_ptr){};

public:
    void CheckExceptionPtr() {
        if (exception_ptr_ != nullptr) {
            std::rethrow_exception(exception_ptr_);
        }
    }

    bool IsFailed() {
        return exception_ptr_ != nullptr;
    }

    void Throw() {
        CheckExceptionPtr();
        throw std::logic_error("No exception");
    }

protected:
    std::exception_ptr exception_ptr_ = nullptr;
};

template <class Function, class... Args>
auto TryRun(Function func, Args... args) {
    using ReturnType = decltype(func(args...));
    try {
        if constexpr (std::is_void_v<ReturnType>) {
            func(args...);
        } else {
            return Try<ReturnType>(func(args...));
        }
    } catch (const std::exception&) {
        return Try<ReturnType>(std::current_exception());
    } catch (const char* string) {
        return Try<ReturnType>(std::logic_error(string));
    } catch (int error_number) {
        return Try<ReturnType>(std::runtime_error(std::strerror(error_number)));
    } catch (...) {
        return Try<ReturnType>(std::logic_error("Unknown exception"));
    }
}
