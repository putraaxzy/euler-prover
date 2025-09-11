#pragma once
#include <complex>
#include <vector>
#include "config.h"

namespace complex_analysis {
    using Real = long double;
    using Complex = std::complex<Real>;
    
    class KahanSum {
        Real sum = 0.0L, compensation = 0.0L;
        
    public:
        void add(Real value);
        Real get() const { return sum; }
    };
    
    Complex exp_taylor_adaptive(Complex z, Real tolerance = config::TAYLOR_CONVERGENCE);
    
    struct ComplexBenchmark {
        size_t samples;
        Real max_absolute_error;
        Real mean_absolute_error;
        Real std_deviation_error;
        Real computation_time_seconds;
        std::vector<Real> error_histogram;
    };
    
    ComplexBenchmark benchmark_euler_formula(size_t num_samples);
    
    // Riemann zeta function for visualization
    inline Complex riemann_zeta(Complex s) {
        // Simple implementation for visualization purposes
        // Valid for Re(s) > 1
        Complex sum = 0.0;
        int max_terms = 1000;
        
        for (int n = 1; n <= max_terms; n++) {
            Complex n_complex(n, 0);
            Complex term = Complex(1.0) / std::pow(n_complex, s);
            sum += term;
            
            // Early termination when terms become sufficiently small
            if (std::abs(term) < 1e-10)
                break;
        }
        
        return sum;
    }
}
