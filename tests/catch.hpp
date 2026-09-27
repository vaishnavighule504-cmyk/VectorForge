#ifndef VECTORFORGE_CATCH_HPP
#define VECTORFORGE_CATCH_HPP

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cmath>

namespace CatchTest {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

inline std::vector<TestCase>& getTestCases() {
    static std::vector<TestCase> cases;
    return cases;
}

inline bool registerTestCase(const std::string& name, std::function<void()> func) {
    getTestCases().push_back({name, func});
    return true;
}

inline int runAllTests() {
    int passed = 0;
    int failed = 0;

    std::cout << "========================================================\n";
    std::cout << "          VectorForge Catch2 Unit Test Runner          \n";
    std::cout << "========================================================\n\n";

    for (const auto& test : getTestCases()) {
        std::cout << "[ RUN      ] " << test.name << "\n";
        try {
            test.func();
            std::cout << "[       OK ] " << test.name << "\n";
            passed++;
        } catch (const std::exception& e) {
            std::cout << "[  FAILED  ] " << test.name << "\n  Reason: " << e.what() << "\n";
            failed++;
        } catch (...) {
            std::cout << "[  FAILED  ] " << test.name << "\n  Reason: Unknown exception\n";
            failed++;
        }
    }

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << "Test Results: " << passed << " passed, " << failed << " failed out of " << (passed + failed) << " tests.\n";
    std::cout << "--------------------------------------------------------\n";

    return failed == 0 ? 0 : 1;
}

} // namespace CatchTest

#define CATCH_CONCAT_IMPL(s1, s2) s1##s2
#define CATCH_CONCAT(s1, s2) CATCH_CONCAT_IMPL(s1, s2)

#define TEST_CASE(name, tags) \
    static void CATCH_CONCAT(test_func_, __LINE__)(); \
    static bool CATCH_CONCAT(test_reg_, __LINE__) = CatchTest::registerTestCase(name, CATCH_CONCAT(test_func_, __LINE__)); \
    static void CATCH_CONCAT(test_func_, __LINE__)()

#define REQUIRE(expr) \
    if (!(expr)) { \
        throw std::runtime_error("REQUIRE failed: " #expr " (File: " __FILE__ ", Line: " + std::to_string(__LINE__) + ")"); \
    }

#define REQUIRE_FALSE(expr) REQUIRE(!(expr))

#define CHECK(expr) REQUIRE(expr)

#endif // VECTORFORGE_CATCH_HPP
