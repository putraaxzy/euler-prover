#pragma once

#include <complex>
#include <vector>
#include <string>
#include <map>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884L
#endif

namespace ultra_precision {

struct MethodResult {
    std::complex<long double> result;
    std::string method_name;
    long double absolute_error;
    long double relative_error;
    double computation_time_ns;
};

struct ComparisonResult {
    std::vector<MethodResult> methods;
    std::map<std::string, std::vector<long double>> error_distribution;
    size_t total_samples;
    long double reference_precision;
};

class EulerMethodComparison {
private:
    int precision_digits;
    long double epsilon;
    
    std::complex<long double> compute_std_library(long double theta);
    std::complex<long double> compute_taylor_series(long double theta);
    std::complex<long double> compute_cordic(long double theta);
    std::complex<long double> compute_arbitrary_precision(long double theta);
    
    std::complex<long double> taylor_exp(std::complex<long double> z, int max_terms);
    std::complex<long double> cordic_rotation(long double angle);
    
    long double compute_absolute_error(const std::complex<long double>& computed, 
                                     const std::complex<long double>& reference);
    long double compute_relative_error(const std::complex<long double>& computed,
                                     const std::complex<long double>& reference);

public:
    explicit EulerMethodComparison(int precision_digits = 50);
    
    ComparisonResult compare_all_methods(long double theta, 
                                       bool run_std = true, 
                                       bool run_taylor = true,
                                       bool run_cordic = true, 
                                       bool run_arbitrary = true);
    
    ComparisonResult batch_comparison(size_t num_samples,
                                    bool run_std = true,
                                    bool run_taylor = true, 
                                    bool run_cordic = true,
                                    bool run_arbitrary = true);
    
    void save_error_histogram(const ComparisonResult& result, const std::string& filename);
};

}