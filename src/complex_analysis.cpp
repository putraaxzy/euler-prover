#include "complex_analysis.h"
#include "rng.h"
#include <cmath>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace complex_analysis {

void KahanSum::add(Real value) {
    Real y = value - compensation;
    Real t = sum + y;
    compensation = (t - sum) - y;
    sum = t;
}

Complex exp_taylor_adaptive(Complex z, Real tolerance) {
    KahanSum real_sum, imag_sum;
    real_sum.add(1.0L);
    
    Complex term(1.0L, 0.0L);
    Real max_term_magnitude = 1.0L;
    
    for (size_t k = 1; k <= config::TAYLOR_MAX_TERMS; k++) {
        term *= z / static_cast<Real>(k);
        
        real_sum.add(term.real());
        imag_sum.add(term.imag());
        
        Real term_magnitude = std::abs(term);
        if (term_magnitude > max_term_magnitude * 10) {
            if (std::abs(z) > 1) {
                int reduction_factor = static_cast<int>(std::ceil(std::log2(std::abs(z))));
                Complex z_reduced = z / static_cast<Real>(1 << reduction_factor);
                Complex result = exp_taylor_adaptive(z_reduced, tolerance);
                
                for (int i = 0; i < reduction_factor; i++) {
                    result *= result;
                }
                return result;
            }
        }
        
        if (term_magnitude < tolerance && k > 10) break;
    }
    
    return Complex(real_sum.get(), imag_sum.get());
}

ComplexBenchmark benchmark_euler_formula(size_t num_samples) {
    ComplexBenchmark benchmark;
    benchmark.samples = num_samples;
    benchmark.error_histogram.resize(100, 0);
    
    SecureRNG rng;
    std::vector<Real> errors;
    errors.reserve(num_samples);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    KahanSum error_sum;
    Real max_error = 0.0L;
    
    // Simple sequential version
    for (size_t i = 0; i < num_samples; i++) {
        Real theta = rng.uniform(-100.0L * M_PI, 100.0L * M_PI);
        theta = std::fmod(theta, 2.0L * M_PI);
        
        Complex reference(std::cos(theta), std::sin(theta));
        Complex test_result = exp_taylor_adaptive(Complex(0.0L, theta));
        
        Real error = std::abs(reference - test_result);
        errors.push_back(error);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    benchmark.computation_time_seconds = std::chrono::duration<Real>(end_time - start_time).count();
    
    for (Real error : errors) {
        error_sum.add(error);
        max_error = std::max(max_error, error);
    }
    
    benchmark.max_absolute_error = max_error;
    benchmark.mean_absolute_error = error_sum.get() / static_cast<Real>(num_samples);
    
    KahanSum variance_sum;
    Real mean = benchmark.mean_absolute_error;
    for (Real error : errors) {
        Real diff = error - mean;
        variance_sum.add(diff * diff);
    }
    benchmark.std_deviation_error = std::sqrt(variance_sum.get() / static_cast<Real>(num_samples - 1));
    
    for (Real error : errors) {
        int bin = static_cast<int>(std::min(99.0L, error / max_error * 99));
        benchmark.error_histogram[bin]++;
    }
    
    return benchmark;
}

}
