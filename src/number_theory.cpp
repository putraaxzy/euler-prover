#include "number_theory.h"
#include "rng.h"
#include <algorithm>
#include <queue>
#include <chrono>
#include <numeric>
#include <map>
#include <iostream>
#include <execution>
#include <thread>
#include <mutex>
#include <atomic>
#include <immintrin.h>
#include <unordered_map>

namespace number_theory {
uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 1) return 0;
    if (exp == 0) return 1;
    
    uint64_t result = 1;
    base %= mod;
    
    while (exp > 0) {
        if (exp & 1) {
            result = ((__uint128_t)result * base) % mod;
        }
        base = ((__uint128_t)base * base) % mod;
        exp >>= 1;
    }
    return result;
}

uint64_t mod_pow_montgomery(uint64_t base, uint64_t exp, const MontgomeryModulus& mont) {
    if (exp == 0) return mont.to_montgomery(1);
    
    uint64_t result = mont.to_montgomery(1);
    uint64_t base_mont = mont.to_montgomery(base);
    
    while (exp > 0) {
        if (exp & 1) {
            result = mont.multiply(result, base_mont);
        }
        base_mont = mont.multiply(base_mont, base_mont);
        exp >>= 1;
    }
    return mont.from_montgomery(result);
}

MontgomeryModulus::MontgomeryModulus(uint64_t modulus) : n(modulus) {
    bits = 64 - __builtin_clzll(n);
    r = 1ULL << bits;
    
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
        uint64_t a = rng.uniform(static_cast<uint64_t>(2), n - 2);
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
        uint64_t x = rng.uniform(static_cast<uint64_t>(2), n - 1);
        uint64_t c = rng.uniform(static_cast<uint64_t>(1), n - 1);
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
    if (n == 2) return 1;
    
    static thread_local std::map<uint64_t, uint64_t> phi_cache;
    
    auto it = phi_cache.find(n);
    if (it != phi_cache.end()) {
        return it->second;
    }
    
    uint64_t result = n;
    uint64_t temp_n = n;
    
    if (temp_n % 2 == 0) {
        result /= 2;
        while (temp_n % 2 == 0) temp_n /= 2;
    }
    
    for (uint64_t i = 3; i * i <= temp_n; i += 2) {
        if (temp_n % i == 0) {
            result = result / i * (i - 1);
            while (temp_n % i == 0) temp_n /= i;
        }
    }
    
    if (temp_n > 1) {
        result = result / temp_n * (temp_n - 1);
    }
    
    phi_cache[n] = result;
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
    auto start_time = std::chrono::high_resolution_clock::now();
    
    const size_t num_threads = std::min(static_cast<size_t>(std::thread::hardware_concurrency()), static_cast<size_t>(8));
    const size_t chunk_size = std::max(static_cast<size_t>(1), (max_n - 1) / num_threads);
    
    std::vector<std::thread> threads;
    std::mutex result_mutex;
    std::atomic<size_t> total_tests{0}, passed_tests{0}, skipped_tests{0};
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> counterexamples;
    
    for (size_t thread_id = 0; thread_id < num_threads; ++thread_id) {
        threads.emplace_back([&, thread_id]() {
            SecureRNG rng(thread_id + 1);
            std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> local_counterexamples;
            
            uint64_t start_n = 2 + thread_id * chunk_size;
            uint64_t end_n = std::min(max_n + 1, start_n + chunk_size);
            
            std::map<uint64_t, uint64_t> phi_cache;
            
            for (uint64_t n = start_n; n < end_n; ++n) {
                if (phi_cache.find(n) == phi_cache.end()) {
                    phi_cache[n] = euler_phi(n);
                }
                
                MontgomeryModulus mont(n);
                
                for (size_t t = 0; t < tests_per_n; ++t) {
                    if (n <= 2) {
                        skipped_tests++;
                        continue;
                    }
                    
                    uint64_t a = rng.uniform(static_cast<uint64_t>(2), n - 1);
                    
                    if (std::__gcd(a, n) != 1) {
                        skipped_tests++;
                        continue;
                    }
                    
                    total_tests++;
                    
                    uint64_t phi_n = phi_cache[n];
                    uint64_t result_val = mod_pow_montgomery(a, phi_n, mont);
                    
                    if (result_val == 1) {
                        passed_tests++;
                    } else {
                        if (local_counterexamples.size() < max_counterexamples / num_threads) {
                            local_counterexamples.emplace_back(a, n, phi_n);
                        }
                    }
                }
            }
            
            std::lock_guard<std::mutex> lock(result_mutex);
            counterexamples.insert(counterexamples.end(), local_counterexamples.begin(), local_counterexamples.end());
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    result.total_tests = total_tests.load();
    result.passed_tests = passed_tests.load();
    result.skipped_tests = skipped_tests.load();
    result.counterexamples = std::move(counterexamples);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.avg_computation_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

#ifdef __AVX2__
void simd_sieve_primes(std::vector<bool>& is_prime, uint64_t limit) {
    if (limit < 2) return;
    
    is_prime.assign(limit + 1, true);
    is_prime[0] = is_prime[1] = false;
    
    for (uint64_t i = 2; i * i <= limit; ++i) {
        if (is_prime[i]) {
            for (uint64_t j = i * i; j <= limit; j += i) {
                is_prime[j] = false;
            }
        }
    }
}
#endif

EulerTestResult batch_test_euler_theorem(uint64_t max_n, size_t tests_per_n, const BatchTestConfig& config) {
    EulerTestResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    size_t num_threads = config.num_threads;
    if (num_threads == 0) {
        num_threads = std::min(static_cast<size_t>(std::thread::hardware_concurrency()), static_cast<size_t>(16));
    }
    
    const uint64_t total_range = max_n - 1;
    const uint64_t batch_size = std::min(config.batch_size, total_range / num_threads + 1);
    
    std::vector<std::thread> threads;
    std::mutex result_mutex;
    std::atomic<size_t> total_tests{0}, passed_tests{0}, skipped_tests{0};
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> counterexamples;
    
    std::atomic<uint64_t> current_batch{2};
    
    for (size_t thread_id = 0; thread_id < num_threads; ++thread_id) {
        threads.emplace_back([&, thread_id]() {
            SecureRNG rng(thread_id * 12345 + 67890);
            std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> local_counterexamples;
            
            std::unordered_map<uint64_t, uint64_t> phi_cache;
            std::unordered_map<uint64_t, std::unique_ptr<MontgomeryModulus>> mont_cache;
            
            while (true) {
                uint64_t batch_start = current_batch.fetch_add(batch_size);
                if (batch_start > max_n) break;
                
                uint64_t batch_end = std::min(batch_start + batch_size, max_n + 1);
                
                for (uint64_t n = batch_start; n < batch_end; ++n) {
                    if (config.enable_caching && phi_cache.find(n) == phi_cache.end()) {
                        phi_cache[n] = euler_phi(n);
                    }
                    
                    if (config.use_montgomery && mont_cache.find(n) == mont_cache.end()) {
                        mont_cache[n] = std::make_unique<MontgomeryModulus>(n);
                    }
                    
                    for (size_t t = 0; t < tests_per_n; ++t) {
                        if (n <= 2) {
                            skipped_tests++;
                            continue;
                        }
                        
                        uint64_t a = rng.uniform(static_cast<uint64_t>(2), n - 1);
                        
                        if (std::__gcd(a, n) != 1) {
                            skipped_tests++;
                            continue;
                        }
                        
                        total_tests++;
                        
                        uint64_t phi_n = config.enable_caching ? phi_cache[n] : euler_phi(n);
                        uint64_t result_val;
                        
                        if (config.use_montgomery && mont_cache[n]) {
                            result_val = mod_pow_montgomery(a, phi_n, *mont_cache[n]);
                        } else {
                            result_val = mod_pow(a, phi_n, n);
                        }
                        
                        if (result_val == 1) {
                            passed_tests++;
                        } else {
                            if (local_counterexamples.size() < 100) {
                                local_counterexamples.emplace_back(a, n, phi_n);
                            }
                        }
                    }
                }
            }
            
            std::lock_guard<std::mutex> lock(result_mutex);
            counterexamples.insert(counterexamples.end(), local_counterexamples.begin(), local_counterexamples.end());
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    result.total_tests = total_tests.load();
    result.passed_tests = passed_tests.load();
    result.skipped_tests = skipped_tests.load();
    result.counterexamples = std::move(counterexamples);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    result.avg_computation_time = std::chrono::duration<double>(end_time - start_time).count();
    
    return result;
}

}
