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
}
