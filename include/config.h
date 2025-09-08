#pragma once
#include <cstddef>
#include <thread>

namespace config {
    constexpr double EPSILON = 1e-18L;
    constexpr size_t MILLER_RABIN_ROUNDS = 20;
    constexpr size_t POLLARD_RHO_MAX_ITER = 1000000;
    constexpr size_t TAYLOR_MAX_TERMS = 500;
    constexpr long double TAYLOR_CONVERGENCE = 1e-25L;
    constexpr size_t MAX_ICOSPHERE_LEVEL = 6;
    
    inline int get_thread_count() {
        return std::thread::hardware_concurrency();
    }
}
