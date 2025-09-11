#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include <tuple>
#include <functional>
#include "config.h"

namespace number_theory {
    using u128 = __uint128_t;
    using i128 = __int128_t;

    #ifdef __AVX2__
    void simd_sieve_primes(std::vector<bool>& is_prime, uint64_t limit);
    #endif
    
    class MontgomeryModulus {
        uint64_t n, r, n_inv, r_squared;
        int bits;
        
    public:
        explicit MontgomeryModulus(uint64_t modulus);
        uint64_t reduce(u128 x) const;
        uint64_t multiply(uint64_t a, uint64_t b) const;
        uint64_t to_montgomery(uint64_t x) const;
        uint64_t from_montgomery(uint64_t x) const;
    };
    
    bool is_prime_miller_rabin(uint64_t n, size_t rounds = config::MILLER_RABIN_ROUNDS);
    uint64_t pollard_rho_factor(uint64_t n);
    std::map<uint64_t, int> factorize_advanced(uint64_t n);
    uint64_t euler_phi(uint64_t n);
    uint64_t carmichael_lambda(uint64_t n);
    
    struct EulerTestResult {
        size_t total_tests = 0;
        size_t passed_tests = 0;
        size_t skipped_tests = 0;
        std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> counterexamples;
        double avg_computation_time = 0.0;
        std::map<uint64_t, size_t> modulus_distribution;
    };
    
    EulerTestResult stress_test_euler_theorem(uint64_t max_n, size_t tests_per_n, size_t max_counterexamples = 100);
    uint64_t mod_pow_montgomery(uint64_t base, uint64_t exp, const MontgomeryModulus& mont);
    
    struct BatchTestConfig {
        uint64_t batch_size = 1000;
        size_t num_threads = 0; 
        bool use_montgomery = true;
        bool enable_caching = true;
    };
    
    EulerTestResult batch_test_euler_theorem(uint64_t max_n, size_t tests_per_n, 
                                           const BatchTestConfig& config = {});
}
