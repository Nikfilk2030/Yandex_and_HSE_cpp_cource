#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

// идиотский нейминг исходит из нежелания снова час дебажить то, что случайно catch переименовал
class InitOneSingleTestBase {
public:
    virtual std::unique_ptr<AbstractTest> Create() = 0;
};

template <class T>
class InitOneSingleTest : public InitOneSingleTestBase {
public:
    InitOneSingleTest() = default;
    std::unique_ptr<AbstractTest> Create() override {
        return std::make_unique<T>();
    }
};

class Substr {
public:
    Substr(const std::string& s) : pattern_(s){};

    bool operator()(const std::string& substr) {
        return substr.find(pattern_) != std::string::npos;
    }

protected:
    std::string pattern_;
};

class FullMatch {
public:
    FullMatch(const std::string& s) : pattern_(s){};

    bool operator()(const std::string& s) {
        return s == pattern_;
    }

protected:
    std::string pattern_;
};

class TestRegistry {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        name_to_test_ptr_[class_name] = std::make_unique<InitOneSingleTest<TestClass>>();
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        if (!name_to_test_ptr_.contains(class_name)) {
            throw std::out_of_range("Unknown name of test");
        }
        return name_to_test_ptr_[class_name]->Create();
    }

    void RunTest(const std::string& test_name) {
        auto current_test = CreateTest(test_name);

        current_test->SetUp();
        try {
            current_test->Run();
            current_test->TearDown();
        } catch (...) {
            current_test->TearDown();
            throw;
        }
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> valid_test_names;
        for (const auto& [name, test_ptr_] : name_to_test_ptr_) {
            if (callback(name)) {
                valid_test_names.push_back(name);
            }
        }
        return valid_test_names;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> test_names;
        for (const auto& [name, test_ptr_] : name_to_test_ptr_) {
            test_names.push_back(name);
        }
        return test_names;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (const auto& [name, test_ptr] : name_to_test_ptr_) {
            if (callback(name)) {
                RunTest(name);
            }
        }
    }

    void Clear() {
        name_to_test_ptr_.clear();
    }

    static TestRegistry& Instance() {
        static TestRegistry test_registry;
        return test_registry;
    }

protected:
    std::map<std::string, std::unique_ptr<InitOneSingleTestBase>> name_to_test_ptr_;
};
