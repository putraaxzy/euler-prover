#include "number_theory.h"
#include "rng.h"
#include <algorithm>
#include <queue>
#include <chrono>
#include <numeric>
#include <map>
#include <iostream>

namespace number_theory {

// Simple modular exponentiation without Montgomery
uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = ((__uint128_t)result * base) % mod;
        base = ((__uint128_t)base * base) % mod;
        exp >>= 1;
    }
    return result;
}

MontgomeryModulus::MontgomeryModulus(uint64_t modulus) : n(modulus) {
    bits = 64 - __builtin_clzll(n);
    r = 1ULL << bits;
    
    // Iterative extended GCD to avoid recursion issues
    i128 old_r = r, r_val = n;
    i128 old_s = 1, s = 0;
    
    while (r_val != 0) {
        i128 quotient = old_r / r_val;
        i128 temp = r_val;
        r_val = old_r - quotient * r_val;
        old_r = temp;
        
        temp = s;
        s = old_s - quotient * s;
        old_s = temp;
    }
    
    i128 inv = old_s;
    n_inv = (-inv) & ((1ULL << bits) - 1);
    r_squared = ((u128)r * r) % n;
}

uint64_t MontgomeryModulus::reduce(u128 x) const {
    uint64_t m = (uint64_t(x) * n_inv) & ((1ULL << bits) - 1);
    u128 t = (x + u128(m) * n) >> bits;
    return t >= n ? t - n : t;
}

uint64_t MontgomeryModulus::multiply(uint64_t a, uint64_t b) const {
    return reduce(u128(a) * b);
}

uint64_t MontgomeryModulus::to_montgomery(uint64_t x) const {
    return reduce(u128(x) * r_squared);
}

uint64_t MontgomeryModulus::from_montgomery(uint64_t x) const {
    return reduce(x);
}

bool is_prime_miller_rabin(uint64_t n, size_t rounds) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;
    
    uint64_t d = n - 1;
    int r = 0;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }
    
    SecureRNG rng;
    
    for (size_t i = 0; i < rounds; i++) {
        uint64_t a = rng.uniform(2ULL, n - 2);
        uint64_t x = mod_pow(a, d, n);
        
        if (x == 1 || x == n - 1) continue;
        
        bool composite = true;
        for (int j = 0; j < r - 1; j++) {
            x = mod_pow(x, 2, n);
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        
        if (composite) return false;
    }
    return true;
}

uint64_t pollard_rho_factor(uint64_t n) {
    if (n % 2 == 0) return 2;
    if (is_prime_miller_rabin(n, config::MILLER_RABIN_ROUNDS)) return n;
    
    SecureRNG rng;
    
    for (int attempt = 0; attempt < 10; attempt++) {
        uint64_t x = rng.uniform(2ULL, n - 1);
        uint64_t c = rng.uniform(1ULL, n - 1);
        uint64_t y = x;
        uint64_t d = 1;
        
        auto f = [c, n](uint64_t x) { 
            return ((u128)x * x + c) % n; 
        };
        
        size_t iter = 0;
        while (d == 1 && iter < config::POLLARD_RHO_MAX_ITER) {
            x = f(x);
            y = f(f(y));
            d = std::__gcd(x > y ? x - y : y - x, n);
            iter++;
        }
        
        if (d != 1 && d != n) return d;
    }
    return n;
}

std::map<uint64_t, int> factorize_advanced(uint64_t n) {
    std::map<uint64_t, int> factors;
    if (n <= 1) return factors;
    
    std::vector<uint64_t> small_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
    for (uint64_t p : small_primes) {
        if (n % p == 0) {
            int count = 0;
            while (n % p == 0) {
                n /= p;
                count++;
            }
            factors[p] = count;
        }
    }
    
    std::queue<uint64_t> to_factor;
    to_factor.push(n);
    
    while (!to_factor.empty()) {
        uint64_t current = to_factor.front();
        to_factor.pop();
        
        if (current == 1) continue;
        if (is_prime_miller_rabin(current, config::MILLER_RABIN_ROUNDS)) {
            factors[current]++;
            continue;
        }
        
        uint64_t factor = pollard_rho_factor(current);
        if (factor == current) {
            for (uint64_t i = 53; i * i <= current; i += 2) {
                if (current % i == 0) {
                    factor = i;
                    break;
                }
            }
            if (factor == current) {
                factors[current]++;
                continue;
            }
        }
        
        to_factor.push(factor);
        to_factor.push(current / factor);
    }
    
    return factors;
}

uint64_t euler_phi(uint64_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    
    auto factors = factorize_advanced(n);
    uint64_t result = n;
    
    for (auto [p, k] : factors) {
        result = result / p * (p - 1);
    }
    
    return result;
}

uint64_t carmichael_lambda(uint64_t n) {
    if (n <= 1) return n;
    if (n == 2) return 1;
    if (n == 4) return 2;
    
    auto factors = factorize_advanced(n);
    uint64_t lambda = 1;
    
    for (auto [p, k] : factors) {
        uint64_t lambda_pk;
        if (p == 2 && k >= 3) {
            lambda_pk = 1ULL << (k - 2);
        } else {
            lambda_pk = (p - 1);
            for (int i = 1; i < k; i++) lambda_pk *= p;
        }
        lambda = std::lcm(lambda, lambda_pk);
    }
    
    return lambda;
}

EulerTestResult stress_test_euler_theorem(uint64_t max_n, size_t tests_per_n, size_t max_counterexamples) {
    EulerTestResult result;
    SecureRNG rng;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simple sequential version - works reliably
    for (uint64_t n = 2; n <= max_n; n++) {
        result.modulus_distribution[n]++;
        
        for (size_t t = 0; t < tests_per_n; t++) {
            // Skip cases where we can't generate valid 'a' in range [2, n-1]
            if (n <= 2) {
                result.skipped_tests++;
                continue;
            }
            
            uint64_t a = rng.uniform(2ULL, n - 1);
            
            if (std::__gcd(a, n) != 1) {
                result.skipped_tests++;
                continue;
            }
            
            result.total_tests++;
            
            uint64_t phi_n = euler_phi(n);
            uint64_t result_val = mod_pow(a, phi_n, n);
            
            if (result_val == 1) {
                result.passed_tests++;
            } else {
                if (result.counterexamples.size() < max_counterexamples) {
                    result.counterexamples.emplace_back(a, n, phi_n);
                }
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.avg_computation_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

}
