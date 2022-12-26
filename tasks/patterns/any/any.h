#pragma once

#include <memory>

class Any {
protected:
    class StorageBase {
    public:
        virtual ~StorageBase() = default;
    };

    template <typename T>
    class Storage : public StorageBase {
    public:
        Storage() = default;
        Storage(const T& value) : value_(value){};
        ~Storage() override = default;

        const T& GetValue() const {
            return value_;
        }

    protected:
        T value_;
    };

public:
    Any() : is_empty_(true){};

    template <class T>
    Any(const T& value) : is_empty_(false), storage_(new Storage<T>(value)){};

    template <class T>
    Any& operator=(const T& value) {
        Clear();
        storage_ = std::make_shared<Storage<T>>(value);
        is_empty_ = false;
        return *this;
    }

    Any(const Any& rhs) {
        Clear();
        if (!rhs.Empty()) {
            storage_ = rhs.storage_;
        }
        is_empty_ = rhs.is_empty_;
    }

    Any& operator=(const Any& rhs) {
        if (!rhs.Empty()) {
            storage_ = rhs.storage_;
        }
        is_empty_ = rhs.is_empty_;
        return *this;
    }

    ~Any() = default;

    bool Empty() const {
        return is_empty_;
    }

    void Clear() {
        if (Empty()) {
            return;
        }
        storage_ = nullptr;
        is_empty_ = true;
    }

    void Swap(Any& rhs) {
        std::swap(storage_, rhs.storage_);
        std::swap(is_empty_, rhs.is_empty_);
    }

    template <class T>
    const T& GetValue() const {
        return (dynamic_cast<const Storage<T>&>(*storage_)).GetValue();
    }

protected:
    bool is_empty_ = true;
    std::shared_ptr<StorageBase> storage_ = nullptr;
};
