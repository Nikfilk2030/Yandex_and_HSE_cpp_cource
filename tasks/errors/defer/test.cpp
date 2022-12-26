#pragma once

#include <memory>

template <typename Callback>
class CallbackStorage {
public:
    explicit CallbackStorage(Callback callback) {
        ::new (GetCallbackBuffer()) Callback(std::move(callback));
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }

    void Delete() {
        if (callback_is_deleted_) {
            return;
        }
        GetCallback().~Callback();
        callback_is_deleted_ = true;
    }

    bool CallbackIsDeleted() {
        return callback_is_deleted_;
    }

    ~CallbackStorage() {
        Delete();
    }

protected:
    bool callback_is_deleted_ = false;
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
};

template <typename Callback>
class Defer final {
public:
    Defer(Callback callback) : callback_storage_(std::move(callback)){};

    void Cancel() && {
        callback_storage_.Delete();
    }

    void Invoke() && {
        if (callback_storage_.CallbackIsDeleted()) {
            return;
        }
        Call();
        callback_storage_.Delete();
    }

    ~Defer() {
        if (callback_storage_.CallbackIsDeleted()) {
            return;
        }
        Call();
    }

protected:
    void Call() {
        std::move(callback_storage_.GetCallback())();
    }

protected:
    CallbackStorage<Callback> callback_storage_;
};
