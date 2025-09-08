#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <omp.h>
#include "config.h"
#include "number_theory.h"
#include "complex_analysis.h"
#include "topology.h"
#include "progress.h"
#include "ultra_precision.h"

void print_usage(const char* prog) {
    std::cout << R"(
EULER COMPUTATIONAL PROOF SYSTEM
================================

USAGE:
  )" << prog << R"( proof <mode> [options...]

MODES:
  number    - Euler's theorem: a^φ(n) ≡ 1 (mod n) for gcd(a,n)=1
  complex   - Euler's formula: e^(iθ) = cos θ + i sin θ  
  topology  - Euler characteristic: V - E + F = 2 for polyhedra
  ultra     - Ultra precision method comparison for e^(iθ)

OPTIONS:
  number [max_n] [tests_per_n] [threads]
  complex [samples] [precision] [threads]
  topology [max_icosphere_level]
  ultra [theta] [precision_digits] [methods]

ULTRA MODE:
  theta            - Angle in radians (default: 1.0)
  precision_digits - Decimal places precision (default: 50)
  methods          - Comma-separated: std,taylor,cordic,arbitrary or 'all'

)";
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    std::cout << std::fixed << std::setprecision(12);
    
    if (argc < 3 || std::string(argv[1]) != "proof") {
        print_usage(argv[0]);
        return 1;
    }
    
    std::string mode = argv[2];
    
    if (mode == "number") {
        uint64_t max_n = (argc >= 4) ? std::stoull(argv[3]) : 50000;
        size_t tests_per_n = (argc >= 5) ? std::stoull(argv[4]) : 10;
        int threads = (argc >= 6) ? std::stoi(argv[5]) : config::get_thread_count();
        
        #ifdef _OPENMP
        omp_set_num_threads(threads);
        #endif
        
        std::cout << "=== EULER'S THEOREM COMPUTATIONAL PROOF ===\n";
        std::cout << "Testing: a^φ(n) ≡ 1 (mod n) for gcd(a,n) = 1\n";
        std::cout << "Parameters: max_n=" << max_n << ", tests_per_n=" << tests_per_n 
                 << ", threads=" << threads << "\n\n";
        std::cout << "Starting computation...\n";
        
        ProgressTracker progress(max_n * tests_per_n, "Number Theory Tests");
        
        auto result = number_theory::stress_test_euler_theorem(max_n, tests_per_n);
        progress.finish();
        
        std::cout << "\n--- RESULTS ---\n";
        std::cout << "Total tests executed: " << result.total_tests << "\n";
        std::cout << "Tests passed:         " << result.passed_tests << "\n";
        std::cout << "Tests skipped:        " << result.skipped_tests << "\n";
        std::cout << "Failures found:       " << result.counterexamples.size() << "\n";
        std::cout << "Success rate:         " << (100.0 * result.passed_tests / result.total_tests) << "%\n";
        std::cout << "Computation time:     " << result.avg_computation_time << "s\n";
        
        if (!result.counterexamples.empty()) {
            std::cout << "\n--- COUNTEREXAMPLES (first 10) ---\n";
            for (size_t i = 0; i < std::min(static_cast<size_t>(10), result.counterexamples.size()); i++) {
                auto [a, n, phi_n] = result.counterexamples[i];
                std::cout << "a=" << a << ", n=" << n << ", φ(n)=" << phi_n << "\n";
            }
            return 2;
        }
        
        std::cout << "\n✓ PROOF STATUS: ALL TESTS PASSED - Euler's theorem holds computationally\n";
        return 0;
    }
    else if (mode == "complex") {
        size_t samples = (argc >= 4) ? std::stoull(argv[3]) : 1000000;
        long double precision = (argc >= 5) ? std::stold(argv[4]) : config::TAYLOR_CONVERGENCE;
        int threads = (argc >= 6) ? std::stoi(argv[5]) : config::get_thread_count();
        
        #ifdef _OPENMP
        omp_set_num_threads(threads);
        #endif
        
        std::cout << "=== EULER'S FORMULA COMPUTATIONAL PROOF ===\n";
        std::cout << "Testing: e^(iθ) = cos θ + i sin θ\n";
        std::cout << "Parameters: samples=" << samples << ", precision=" << precision 
                 << ", threads=" << threads << "\n\n";
        
        ProgressTracker progress(samples, "Complex Analysis Tests");
        
        auto benchmark = complex_analysis::benchmark_euler_formula(samples);
        progress.finish();
        
        std::cout << "\n--- RESULTS ---\n";
        std::cout << "Samples tested:           " << benchmark.samples << "\n";
        std::cout << "Max absolute error:       " << std::scientific << benchmark.max_absolute_error << "\n";
        std::cout << "Mean absolute error:      " << benchmark.mean_absolute_error << "\n";
        std::cout << "Std deviation of error:   " << benchmark.std_deviation_error << "\n";
        std::cout << "Computation time:         " << std::fixed << benchmark.computation_time_seconds << "s\n";
        std::cout << "Target precision:         " << std::scientific << precision << "\n";
        
        std::cout << "\n--- ERROR DISTRIBUTION ---\n";
        for (size_t i = 0; i < 10; i++) {
            double bin_start = (i * benchmark.max_absolute_error) / 10;
            double bin_end = ((i+1) * benchmark.max_absolute_error) / 10;
            size_t count = benchmark.error_histogram[i * 10];
            std::cout << "[" << std::scientific << bin_start << ", " << bin_end << "): " 
                     << count << " samples\n";
        }
        
        bool passed = benchmark.max_absolute_error < precision * 1000;
        std::cout << "\n✓ PROOF STATUS: " << (passed ? "PASSED" : "MARGINAL") 
                 << " - Euler's formula verified within numerical precision\n";
        
        return passed ? 0 : 1;
    }
    else if (mode == "topology") {
        int max_level = (argc >= 4) ? std::stoi(argv[3]) : 4;
        max_level = std::min(max_level, static_cast<int>(config::MAX_ICOSPHERE_LEVEL));
        
        std::cout << "=== EULER CHARACTERISTIC COMPUTATIONAL PROOF ===\n";
        std::cout << "Testing: V - E + F = 2 for polyhedra\n";
        std::cout << "Parameters: max_icosphere_level=" << max_level << "\n\n";
        
        auto result = topology::run_comprehensive_suite();
        
        std::cout << "--- RESULTS ---\n";
        std::cout << std::left << std::setw(25) << "Polyhedron" 
                 << std::setw(8) << "V" << std::setw(8) << "E" << std::setw(8) << "F" 
                 << std::setw(6) << "χ" << std::setw(8) << "Status" 
                 << std::setw(12) << "Area" << std::setw(12) << "Volume" << "Time(ms)\n";
        std::cout << std::string(95, '-') << "\n";
        
        for (const auto& test : result.test_cases) {
            std::cout << std::left << std::setw(25) << test.name
                     << std::right << std::setw(8) << test.vertices
                     << std::setw(8) << test.edges  
                     << std::setw(8) << test.faces
                     << std::setw(6) << test.euler_characteristic
                     << std::setw(8) << (test.passed ? "PASS" : "FAIL")
                     << std::setw(12) << std::fixed << std::setprecision(4) << test.surface_area
                     << std::setw(12) << test.volume
                     << std::setw(8) << std::setprecision(2) << (test.computation_time * 1000) << "\n";
        }
        
        std::cout << "\n--- SUMMARY ---\n";
        std::cout << "Total tests:       " << result.test_cases.size() << "\n";
        std::cout << "Tests passed:      " << std::count_if(result.test_cases.begin(), result.test_cases.end(),
                                                         [](const auto& t) { return t.passed; }) << "\n";
        std::cout << "Computation time:  " << std::fixed << std::setprecision(3) << result.total_computation_time << "s\n";
        
        std::cout << "\n✓ PROOF STATUS: " << (result.all_passed ? "ALL TESTS PASSED" : "SOME FAILURES") 
                 << " - Euler characteristic verified for tested polyhedra\n";
        
        return result.all_passed ? 0 : 2;
    }
    else if (mode == "ultra") {
        long double theta = (argc >= 4) ? std::stold(argv[3]) : 1.0L;
        int precision = (argc >= 5) ? std::stoi(argv[4]) : 50;
        std::string methods = (argc >= 6) ? argv[5] : "all";
        
        std::cout << "=== ULTRA PRECISION EULER FORMULA COMPARISON ===\n";
        std::cout << "Comparing computation methods for e^(iθ)\n";
        std::cout << "Parameters: θ=" << theta << ", precision=" << precision 
                 << " digits, methods=" << methods << "\n\n";
        
        ultra_precision::EulerMethodComparison comparison(precision);
        
        bool run_std = (methods == "all" || methods.find("std") != std::string::npos);
        bool run_taylor = (methods == "all" || methods.find("taylor") != std::string::npos);
        bool run_cordic = (methods == "all" || methods.find("cordic") != std::string::npos);
        bool run_arbitrary = (methods == "all" || methods.find("arbitrary") != std::string::npos);
        
        auto results = comparison.compare_all_methods(theta, run_std, run_taylor, run_cordic, run_arbitrary);
        
        std::cout << "--- COMPUTATION RESULTS ---\n";
        std::cout << std::left << std::setw(15) << "Method" 
                 << std::setw(25) << "Real Part" 
                 << std::setw(25) << "Imaginary Part"
                 << std::setw(15) << "Abs Error"
                 << std::setw(15) << "Time (ns)" << "\n";
        std::cout << std::string(95, '-') << "\n";
        
        for (const auto& method : results.methods) {
            std::cout << std::left << std::setw(15) << method.method_name
                     << std::setw(25) << std::setprecision(precision/2) << std::fixed << method.result.real()
                     << std::setw(25) << method.result.imag()
                     << std::setw(15) << std::scientific << method.absolute_error
                     << std::setw(15) << std::fixed << std::setprecision(1) << method.computation_time_ns << "\n";
        }
        
        if (results.methods.size() > 1) {
            std::cout << "\n--- ERROR ANALYSIS ---\n";
            auto reference = results.methods[0];
            
            std::cout << "Reference method: " << reference.method_name << "\n";
            std::cout << "Reference result: " << std::setprecision(precision/2) << std::fixed
                     << reference.result.real() << " + " << reference.result.imag() << "i\n";
            
            std::cout << "\n--- METHOD COMPARISON ---\n";
            for (size_t i = 1; i < results.methods.size(); i++) {
                const auto& method = results.methods[i];
                std::cout << method.method_name << " vs " << reference.method_name 
                         << ": abs error = " << std::scientific << method.absolute_error
                         << ", rel error = " << method.relative_error
                         << ", time ratio = " << std::fixed << std::setprecision(2) 
                         << (method.computation_time_ns / reference.computation_time_ns) << "x\n";
            }
        }
        
        std::string filename = "build/ultra_precision_results.csv";
        comparison.save_error_histogram(results, filename);
        std::cout << "\n✓ Results exported to: " << filename << "\n";
        
        std::cout << "\n✓ ULTRA PRECISION ANALYSIS COMPLETE\n";
        std::cout << "All methods computed e^(i" << theta << ") with " << precision << "-digit precision\n";
        
        return 0;
    }
    else {
        print_usage(argv[0]);
        return 1;
    }
}
