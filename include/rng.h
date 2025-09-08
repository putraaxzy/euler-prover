#pragma once
#include <array>
#include <chrono>
#include <mutex>
#include <type_traits>
#include "config.h"

class SecureRNG {
private:
    std::array<uint64_t, 4> state;
    mutable std::mutex mtx;
    
public:
    explicit SecureRNG(uint64_t seed = std::chrono::high_resolution_clock::now().time_since_epoch().count());
    
    uint64_t next();
    
    template<typename T>
    T uniform(T min_val, T max_val) {
        if constexpr (std::is_integral_v<T>) {
            return min_val + (next() % (max_val - min_val + 1));
        } else {
            return min_val + (max_val - min_val) * (next() >> 11) * (1.0 / 9007199254740992.0);
        }
    }
    
private:
    static constexpr uint64_t rotl(uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }
};
