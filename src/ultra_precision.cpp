#include "ultra_precision.h"
#include <cmath>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace ultra_precision {

EulerMethodComparison::EulerMethodComparison(int precision_digits) 
    : precision_digits(precision_digits) {
    epsilon = std::pow(10.0L, -static_cast<long double>(precision_digits));
}

ComparisonResult EulerMethodComparison::compare_all_methods(
    long double theta, bool run_std, bool run_taylor, bool run_cordic, bool run_arbitrary) {
    
    ComparisonResult result;
    result.total_samples = 1;
    result.reference_precision = static_cast<long double>(precision_digits);
    
    // Compute reference value (highest precision available)
    std::complex<long double> reference = compute_taylor_series(theta);
    
    auto measure_method = [&](auto method, const std::string& name) -> MethodResult {
        auto start = std::chrono::high_resolution_clock::now();
        auto computed = method(theta);
        auto end = std::chrono::high_resolution_clock::now();
        
        MethodResult method_result;
        method_result.result = computed;
        method_result.method_name = name;
        method_result.absolute_error = compute_absolute_error(computed, reference);
        method_result.relative_error = compute_relative_error(computed, reference);
        method_result.computation_time_ns = std::chrono::duration<double, std::nano>(end - start).count();
        
        return method_result;
    };
    
    if (run_std) {
        result.methods.push_back(measure_method(
            [this](long double t) { return compute_std_library(t); }, 
            "std::exp"
        ));
    }
    
    if (run_taylor) {
        result.methods.push_back(measure_method(
            [this](long double t) { return compute_taylor_series(t); },
            "Taylor Series"
        ));
    }
    
    if (run_cordic) {
        result.methods.push_back(measure_method(
            [this](long double t) { return compute_cordic(t); },
            "CORDIC"
        ));
    }
    
    if (run_arbitrary) {
        result.methods.push_back(measure_method(
            [this](long double t) { return compute_arbitrary_precision(t); },
            "Arbitrary Precision"
        ));
    }
    
    return result;
}

ComparisonResult EulerMethodComparison::batch_comparison(
    size_t num_samples, bool run_std, bool run_taylor, bool run_cordic, bool run_arbitrary) {
    
    ComparisonResult final_result;
    final_result.total_samples = num_samples;
    final_result.reference_precision = static_cast<long double>(precision_digits);
    
    std::vector<std::string> method_names;
    if (run_std) method_names.push_back("std::exp");
    if (run_taylor) method_names.push_back("Taylor Series");
    if (run_cordic) method_names.push_back("CORDIC");
    if (run_arbitrary) method_names.push_back("Arbitrary Precision");
    
    for (const auto& name : method_names) {
        final_result.error_distribution[name] = std::vector<long double>();
        final_result.error_distribution[name].reserve(num_samples);
    }
    
    std::vector<MethodResult> accumulated_results(method_names.size());
    for (size_t i = 0; i < accumulated_results.size(); i++) {
        accumulated_results[i].method_name = method_names[i];
        accumulated_results[i].absolute_error = 0;
        accumulated_results[i].relative_error = 0;
        accumulated_results[i].computation_time_ns = 0;
    }
    
    for (size_t i = 0; i < num_samples; i++) {
        long double theta = static_cast<long double>(i) * 2.0L * M_PI / num_samples;
        auto single_result = compare_all_methods(theta, run_std, run_taylor, run_cordic, run_arbitrary);
        
        for (size_t j = 0; j < single_result.methods.size(); j++) {
            const auto& method_result = single_result.methods[j];
            accumulated_results[j].absolute_error += method_result.absolute_error;
            accumulated_results[j].relative_error += method_result.relative_error;
            accumulated_results[j].computation_time_ns += method_result.computation_time_ns;
            
            final_result.error_distribution[method_result.method_name].push_back(
                method_result.absolute_error
            );
        }
    }
    
    for (auto& result : accumulated_results) {
        result.absolute_error /= num_samples;
        result.relative_error /= num_samples;
        result.computation_time_ns /= num_samples;
    }
    
    final_result.methods = accumulated_results;
    return final_result;
}

std::complex<long double> EulerMethodComparison::compute_std_library(long double theta) {
    return std::exp(std::complex<long double>(0, theta));
}

std::complex<long double> EulerMethodComparison::compute_taylor_series(long double theta) {
    return taylor_exp(std::complex<long double>(0, theta), precision_digits * 2);
}

std::complex<long double> EulerMethodComparison::compute_cordic(long double theta) {
    return cordic_rotation(theta);
}

std::complex<long double> EulerMethodComparison::compute_arbitrary_precision(long double theta) {
    return taylor_exp(std::complex<long double>(0, theta), precision_digits * 4);
}

std::complex<long double> EulerMethodComparison::taylor_exp(std::complex<long double> z, int max_terms) {
    std::complex<long double> result(1, 0);
    std::complex<long double> term(1, 0);
    
    for (int n = 1; n < max_terms; n++) {
        term *= z / static_cast<long double>(n);
        result += term;
        
        if (std::abs(term) < epsilon) break;
    }
    
    return result;
}

std::complex<long double> EulerMethodComparison::cordic_rotation(long double angle) {
    const int iterations = precision_digits + 10;
    long double x = 0.60725293500888125617L;
    long double y = 0;
    long double z = angle;
    
    std::vector<long double> atan_table;
    for (int i = 0; i < iterations; i++) {
        atan_table.push_back(std::atan(std::pow(2.0L, -i)));
    }
    
    for (int i = 0; i < iterations; i++) {
        long double sigma = (z >= 0) ? 1 : -1;
        long double x_new = x - sigma * y * std::pow(2.0L, -i);
        long double y_new = y + sigma * x * std::pow(2.0L, -i);
        long double z_new = z - sigma * atan_table[i];
        
        x = x_new;
        y = y_new;
        z = z_new;
    }
    
    return std::complex<long double>(x, y);
}

long double EulerMethodComparison::compute_absolute_error(
    const std::complex<long double>& computed, 
    const std::complex<long double>& reference) {
    return std::abs(computed - reference);
}

long double EulerMethodComparison::compute_relative_error(
    const std::complex<long double>& computed,
    const std::complex<long double>& reference) {
    long double ref_abs = std::abs(reference);
    if (ref_abs < epsilon) return 0;
    return std::abs(computed - reference) / ref_abs;
}

void EulerMethodComparison::save_error_histogram(const ComparisonResult& result, const std::string& filename) {
    std::ofstream file(filename);
    file << std::fixed << std::setprecision(precision_digits);
    
    file << "# Ultra Precision Euler Formula Method Comparison\n";
    file << "# Samples: " << result.total_samples << "\n";
    file << "# Precision: " << result.reference_precision << " digits\n\n";
    
    file << "Method,Mean_Abs_Error,Std_Dev_Error,Min_Error,Max_Error,Mean_Time_ns\n";
    
    for (const auto& method : result.methods) {
        if (result.error_distribution.find(method.method_name) != result.error_distribution.end()) {
            const auto& errors = result.error_distribution.at(method.method_name);
            
            long double mean_error = std::accumulate(errors.begin(), errors.end(), 0.0L) / errors.size();
            long double variance = 0;
            for (auto err : errors) {
                variance += (err - mean_error) * (err - mean_error);
            }
            variance /= errors.size();
            long double std_dev = std::sqrt(variance);
            
            long double min_error = *std::min_element(errors.begin(), errors.end());
            long double max_error = *std::max_element(errors.begin(), errors.end());
            
            file << method.method_name << "," 
                 << mean_error << "," 
                 << std_dev << ","
                 << min_error << ","
                 << max_error << ","
                 << method.computation_time_ns << "\n";
        } else {
            file << method.method_name << "," 
                 << method.absolute_error << "," 
                 << "0,"
                 << method.absolute_error << ","
                 << method.absolute_error << ","
                 << method.computation_time_ns << "\n";
        }
    }
    
    if (result.total_samples > 1 && !result.error_distribution.empty()) {
        file << "\n# Error Distribution Data\n";
        file << "Sample";
        for (const auto& method : result.methods) {
            file << "," << method.method_name;
        }
        file << "\n";
        
        for (size_t i = 0; i < result.total_samples; i++) {
            file << i;
            for (const auto& method : result.methods) {
                if (result.error_distribution.find(method.method_name) != result.error_distribution.end()) {
                    const auto& errors = result.error_distribution.at(method.method_name);
                    if (i < errors.size()) {
                        file << "," << errors[i];
                    } else {
                        file << ",0";
                    }
                } else {
                    file << ",0";
                }
            }
            file << "\n";
        }
    }
}

}
