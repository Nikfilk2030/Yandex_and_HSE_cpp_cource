#pragma once

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>

class ICommand {
public:
    virtual void Do() = 0;
    virtual void Undo() = 0;

    virtual ~ICommand() = default;
};

class Editor {
public:
    const std::string& GetText() {
        return text_;
    }

    void Type(char c);

    void ShiftLeft();

    void ShiftRight();

    void Backspace();

    void Undo();

    void Redo();

protected:
    size_t cursor_ = 0;
    std::string text_;
    std::vector<std::shared_ptr<ICommand>> commands_;
    std::vector<std::shared_ptr<ICommand>> declined_commands_;
};

class TypeCommand : public ICommand {
public:
    TypeCommand(size_t* cursor, std::string* text, char chr)
        : cursor_ptr_(cursor), text_ptr_(text), chr_(chr){};

    void Do() {
        text_ptr_->insert(*cursor_ptr_, 1, chr_);
        ++(*cursor_ptr_);
    }

    void Undo() {
        if (*cursor_ptr_ != 0) {
            --(*cursor_ptr_);
        }
        text_ptr_->erase(*cursor_ptr_, 1);
    }

protected:
    size_t* cursor_ptr_;
    std::string* text_ptr_;
    char chr_;
};

class ShiftLeftCommand : public ICommand {
public:
    ShiftLeftCommand(size_t* cursor, std::string* text) : cursor_ptr_(cursor), text_ptr_(text){};

    void Do() {
        if (*cursor_ptr_ != 0) {
            --(*cursor_ptr_);
        }
    }

    void Undo() {
        *cursor_ptr_ = std::min(text_ptr_->size(), *cursor_ptr_ + 1);
    }

protected:
    size_t* cursor_ptr_;
    std::string* text_ptr_;
};

class ShiftRightCommand : public ICommand {
public:
    ShiftRightCommand(size_t* cursor, std::string* text) : cursor_ptr_(cursor), text_ptr_(text){};

    void Do() {
        *cursor_ptr_ = std::min(text_ptr_->size(), *cursor_ptr_ + 1);
    }

    void Undo() {
        if (*cursor_ptr_ != 0) {
            --(*cursor_ptr_);
        }
    }

protected:
    size_t* cursor_ptr_;
    std::string* text_ptr_;
};

class BackSpaceCommand : public ICommand {
public:
    BackSpaceCommand(Editor& editor, size_t* cursor, std::string* text)
        : editor_(editor), cursor_ptr_(cursor), text_ptr_(text){};

    void Do() {
        if (*cursor_ptr_ == 0) {
            return;
        }
        --(*cursor_ptr_);
        erased_char_ = text_ptr_->at(*cursor_ptr_);
        text_ptr_->erase(*cursor_ptr_, 1);
    }

    void Undo() {
        text_ptr_->insert(*cursor_ptr_, 1, erased_char_);
        ++(*cursor_ptr_);
    }

protected:
    Editor& editor_;
    char erased_char_;
    size_t* cursor_ptr_;
    std::string* text_ptr_;
};

void Editor::Type(char c) {
    declined_commands_.clear();
    commands_.push_back(std::make_shared<TypeCommand>(&cursor_, &text_, c));
    commands_.back()->Do();
}

void Editor::ShiftLeft() {
    if (cursor_ == 0) {
        return;
    }
    commands_.push_back(std::make_shared<ShiftLeftCommand>(&cursor_, &text_));
    commands_.back()->Do();
}

void Editor::ShiftRight() {
    if (cursor_ == text_.size()) {
        return;
    }
    commands_.push_back(std::make_shared<ShiftRightCommand>(&cursor_, &text_));
    commands_.back()->Do();
}

void Editor::Backspace() {
    if (cursor_ == 0) {
        return;
    }
    commands_.push_back(std::make_shared<BackSpaceCommand>(*this, &cursor_, &text_));
    commands_.back()->Do();
}

void Editor::Undo() {
    if (commands_.empty()) {
        return;
    }
    auto& command = commands_.back();
    command->Undo();
    declined_commands_.push_back(command);
    commands_.pop_back();
}

void Editor::Redo() {
    if (declined_commands_.empty()) {
        return;
    }
    auto& declined_command = declined_commands_.back();
    declined_command->Do();
    commands_.push_back(declined_command);
    declined_commands_.pop_back();
}
