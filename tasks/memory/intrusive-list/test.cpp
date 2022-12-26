#pragma once

#include <algorithm>

// dummy->prev - конец
// dummy->next - начало

class ListHook {
public:
    ListHook() = default;

    bool IsLinked() const {
        return !(prev_ == nullptr || next_ == nullptr);  // хз зачем тут next, путь будет
    }

    void Unlink() {
        if (!this->IsLinked()) {
            return;
        }
        prev_->next_ = next_;
        next_->prev_ = prev_;
        next_ = nullptr;
        prev_ = nullptr;
    }

    // Must unlink element from list
    ~ListHook() {
        this->Unlink();
    }

    ListHook(const ListHook&) = delete;

private:
    template <class T>
    friend class List;

    // that helper function might be useful
    void LinkBefore(ListHook* other) {
        other->next_ = this;
        other->prev_ = this->prev_;
        this->prev_->next_ = other;
        this->prev_ = other;
    }

protected:
    ListHook* prev_ = nullptr;
    ListHook* next_ = nullptr;
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator(ListHook* ptr) {
            current_ = ptr;
        }

        Iterator& operator++() {
            current_ = current_->next_;
            return *this;
        }
        Iterator operator++(int) {
            auto tmp = *this;
            current_ = current_->next_;
            return tmp;
        }

        T& operator*() const {
            return *static_cast<T*>(current_);
        }

        T* operator->() const {
            return static_cast<T*>(current_);
        }

        bool operator==(const Iterator& rhs) const {
            return current_ == rhs.current_;
        }
        bool operator!=(const Iterator& rhs) const {
            return current_ != rhs.current_;
        }

    public:
        ListHook* current_ = nullptr;
    };

    List() {
        dummy_ = new ListHook;
        dummy_->next_ = dummy_;
        dummy_->prev_ = dummy_;
    }
    List(const List&) = delete;
    // ТЕСТОВЫЙ ВАРИАНТ, кажется ещё надо удалить other
    List(List&& other) {
        if (this == &other) {
            return;
        }
        std::swap(dummy_, other.dummy_);
        other.dummy_ = nullptr;
    }

    // must unlink all elements from list
    ~List() {
        ListHook* current = dummy_;
        while (current != nullptr) {
            ListHook* next = current->next_;
            current->Unlink();
            current = next;
        }
        delete dummy_;
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        if (this == &other) {
            return *this;
        }
        std::swap(this->dummy_, other.dummy_);
        other.dummy_ = nullptr;
        return *this;
    }

    bool IsEmpty() const {
        if (dummy_ == nullptr) {
            return true;
        }
        return dummy_->next_ == dummy_;
    }
    // that method is allowed to be O(n)
    size_t Size() const {
        if (dummy_ == nullptr) {
            return 0;
        }
        size_t size = 0;
        ListHook* next = dummy_->next_;
        while (next != dummy_) {
            ++size;
            next = next->next_;
        }
        return size;
    }

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem) {
        dummy_->LinkBefore(elem);
    }
    void PushFront(T* elem) {
        dummy_->next_->LinkBefore(elem);
    }

    T& Front() {
        return reinterpret_cast<T&>(*dummy_->next_);
    }
    const T& Front() const {
        return reinterpret_cast<T&>(*dummy_->next_);
    }

    T& Back() {
        return reinterpret_cast<T&>(*dummy_->prev_);
    }
    const T& Back() const {
        return reinterpret_cast<T&>(*dummy_->prev_);
    }

    void PopBack() {
        dummy_->prev_->Unlink();
    }
    void PopFront() {
        dummy_->next_->Unlink();
    }

    Iterator Begin() {
        return Iterator(dummy_);
    }
    Iterator End() {
        return Iterator(dummy_);
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(element);
    }

protected:
    ListHook* dummy_ = nullptr;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
