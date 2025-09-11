#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>
#include <thread>
#include <chrono>
#include "config.h"
#include "number_theory.h"
#include "complex_analysis.h"
#include "topology.h"
#include "progress.h"
#ifndef NO_VISUALIZATION
#include "visualization.h"
#endif

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

void print_usage(const char* prog) {
    std::cout << "\n+=======================================+\n";
    std::cout << "| EULER COMPUTATIONAL PROOF SYSTEM      |\n";
    std::cout << "+=======================================+\n\n";
    
    std::cout << "USAGE:\n";
    std::cout << "  " << prog << " <mode> [options...]\n";
    std::cout << "  " << prog << " visualize <mode> [options...]\n\n";
    
    std::cout << "COMPUTATIONAL MODES:\n";
    std::cout << "  number    - Euler's theorem: a^φ(n) ≡ 1 (mod n) for gcd(a,n)=1\n";
    std::cout << "  complex   - Euler's formula: e^(iθ) = cos θ + i sin θ  \n";
    std::cout << "  topology  - Euler characteristic: V - E + F = 2 for polyhedra\n";
    std::cout << "  ultra     - Ultra precision method comparison for e^(iθ)\n\n";
    
    std::cout << "VISUALIZATION MODES:\n";
    std::cout << "  euler     - Visualize Euler's formula in 3D\n";
    std::cout << "  topology  - Visualize polyhedra and Euler characteristic\n";
    std::cout << "  complex   - Visualize complex functions with domain coloring\n";
    std::cout << "  primes    - Visualize prime number distributions\n\n";
    
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << prog << " number 10000 20        # Test Euler's theorem up to n=10000\n";
    std::cout << "  " << prog << " complex 1000000 1e-12  # Test Euler's formula with high precision\n";
    std::cout << "  " << prog << " visualize topology icosphere 4  # Visualize level 4 icosphere\n";
    std::cout << "  " << prog << " viz complex euler 800   # Visualize Euler's formula at 800x800 resolution\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

    if (mode == "number") {
        std::cout << "\n+=======================================+\n";
        std::cout << "| EULER'S THEOREM COMPUTATIONAL PROOF   |\n";
        std::cout << "+=======================================+\n";
        std::cout << "Testing: a^φ(n) ≡ 1 (mod n) for gcd(a,n) = 1\n";

        int max_n = (argc > 2) ? std::stoi(argv[2]) : 1000;
        int tests_per_n = (argc > 3) ? std::stoi(argv[3]) : 10;
        int num_threads = (argc > 4) ? std::stoi(argv[4]) : std::thread::hardware_concurrency();

        std::cout << "Parameters: max_n=" << max_n << ", tests_per_n=" << tests_per_n 
                  << ", threads=" << num_threads << "\n\n";

        auto start_time = std::chrono::high_resolution_clock::now();
        std::cout << "Starting computation...\n";

        int total_tests = 0;
        int passed_tests = 0;
        int failed_tests = 0;
        int skipped_tests = 0;

        ProgressTracker progress(max_n - 1, "Number Theory Tests");

        for (int n = 2; n <= max_n; ++n) {
            for (int test = 0; test < tests_per_n; ++test) {
                int a = 2 + (std::rand() % (n - 1));
                
                if (std::__gcd(a, n) != 1) {
                    skipped_tests++;
                    continue;
                }

                uint64_t phi_n = number_theory::euler_phi(n);
                uint64_t result = mod_pow(a, phi_n, n);
                
                total_tests++;
                if (result == 1) {
                    passed_tests++;
                } else {
                    failed_tests++;
                }
            }
            progress.update(n - 1);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

        std::cout << "\n+---------------------------------+\n";
        std::cout << "|          RESULTS                |\n";
        std::cout << "+---------------------------------+\n";
        std::cout << "Total tests executed: " << total_tests << "\n";
        std::cout << "Tests passed:         " << passed_tests << "\n";
        std::cout << "Tests skipped:        " << skipped_tests << "\n";
        std::cout << "Failures found:       " << failed_tests << "\n";
        std::cout << "Success rate:         " << (total_tests > 0 ? (100.0 * passed_tests / total_tests) : 0) << "%\n";
        std::cout << "Computation time:     " << duration.count() << "s\n\n";

        if (failed_tests == 0) {
            std::cout << "✓ PROOF STATUS: ALL TESTS PASSED - Euler's theorem holds computationally\n";
            return 0;
        } else {
            std::cout << "✗ PROOF STATUS: " << failed_tests << " FAILURES DETECTED\n";
            return 1;
        }
    }
    
    else if (mode == "complex") {
        std::cout << "\n+=======================================+\n";
        std::cout << "| EULER'S FORMULA COMPUTATIONAL PROOF   |\n";
        std::cout << "+=======================================+\n";
        std::cout << "Testing: e^(iθ) = cos θ + i sin θ\n";

        int samples = (argc > 2) ? std::stoi(argv[2]) : 1000000;
        double precision = (argc > 3) ? std::stod(argv[3]) : 1e-12;
        int num_threads = (argc > 4) ? std::stoi(argv[4]) : std::thread::hardware_concurrency();

        std::cout << "Parameters: samples=" << samples << ", precision=" << precision 
                  << ", threads=" << num_threads << "\n\n";

        auto start_time = std::chrono::high_resolution_clock::now();
        std::cout << "Starting computation...\n";

        int passed_tests = 0;
        int failed_tests = 0;
        double max_error = 0.0;

        ProgressTracker progress(samples, "Complex Analysis Tests");

        for (int i = 0; i < samples; ++i) {
            double theta = -10.0 + 20.0 * i / samples;
            
            complex_analysis::Complex euler_result = complex_analysis::exp_taylor_adaptive(complex_analysis::Complex(0, theta));
            complex_analysis::Complex expected(std::cos(theta), std::sin(theta));
            
            double error = std::abs(euler_result - expected);
            max_error = std::max(max_error, error);
            
            if (error < precision) {
                passed_tests++;
            } else {
                failed_tests++;
            }
            
            if (i % 10000 == 0) progress.update(i);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);

        std::cout << "\n+---------------------------------+\n";
        std::cout << "|          RESULTS                |\n";
        std::cout << "+---------------------------------+\n";
        std::cout << "Total tests executed: " << samples << "\n";
        std::cout << "Tests passed:         " << passed_tests << "\n";
        std::cout << "Failures found:       " << failed_tests << "\n";
        std::cout << "Maximum error:        " << std::scientific << max_error << "\n";
        std::cout << "Success rate:         " << (100.0 * passed_tests / samples) << "%\n";
        std::cout << "Computation time:     " << std::fixed << duration.count() << "s\n\n";

        if (failed_tests == 0) {
            std::cout << "✓ PROOF STATUS: ALL TESTS PASSED - Euler's formula holds computationally\n";
            return 0;
        } else {
            std::cout << "✗ PROOF STATUS: " << failed_tests << " FAILURES DETECTED\n";
            return 1;
        }
    }
    
    else if (mode == "topology") {
        std::cout << "\n+=======================================+\n";
        std::cout << "| EULER CHARACTERISTIC PROOF            |\n";
        std::cout << "+=======================================+\n";
        std::cout << "Testing: V - E + F = 2 for polyhedra\n";

        int max_level = (argc > 2) ? std::stoi(argv[2]) : 4;
        std::cout << "Parameters: max_icosphere_level=" << max_level << "\n\n";

        std::cout << "Starting computation...\n";

        bool all_passed = true;
        for (int level = 0; level <= max_level; ++level) {
            topology::IcosphereGenerator generator;
            topology::TopologicalMesh icosphere = generator.generate(level);
            
            auto [V, E, F, euler_char] = icosphere.euler_characteristic();
            
            std::cout << "Level " << level << ": V=" << V << " E=" << E << " F=" << F 
                      << " χ=" << euler_char;
            
            if (euler_char == 2) {
                std::cout << " ✓\n";
            } else {
                std::cout << " ✗\n";
                all_passed = false;
            }
        }

        std::cout << "\n+---------------------------------+\n";
        std::cout << "|          RESULTS                |\n";
        std::cout << "+---------------------------------+\n";
        
        if (all_passed) {
            std::cout << "✓ PROOF STATUS: ALL TESTS PASSED - Euler characteristic holds\n";
            return 0;
        } else {
            std::cout << "✗ PROOF STATUS: FAILURES DETECTED\n";
            return 1;
        }
    }
    
    else if (mode == "visualize" || mode == "viz") {
#ifdef NO_VISUALIZATION
        std::cout << "\n+=======================================+\n";
        std::cout << "| VISUALIZATION NOT AVAILABLE           |\n";
        std::cout << "+=======================================+\n";
        std::cout << "This build was compiled without visualization support.\n";
        std::cout << "Use the computational modes instead:\n";
        std::cout << "  " << argv[0] << " number 1000 10\n";
        std::cout << "  " << argv[0] << " complex 10000 1e-12\n";
        std::cout << "  " << argv[0] << " topology 4\n";
        return 0;
#else
        if (argc < 3) {
            std::cout << "Error: Visualization mode requires a submode\n";
            print_usage(argv[0]);
            return 1;
        }

        std::string submode = argv[2];
        std::transform(submode.begin(), submode.end(), submode.begin(), ::tolower);

        visualization::VisualizationConfig config;
        config.width = 800;
        config.height = 600;
        config.outputFilePath = "euler_result.ppm";
        
        visualization::Visualizer3D visualizer(config);

        if (submode == "topology") {
            std::string shape = (argc > 3) ? argv[3] : "icosphere";
            int level = (argc > 4) ? std::stoi(argv[4]) : 3;

            if (shape == "icosphere") {
                topology::IcosphereGenerator generator;
                topology::TopologicalMesh mesh = generator.generate(level);
                std::cout << "Visualizing icosphere level " << level << "\n";
                visualizer.renderEulerCharacteristic(mesh);
            } else if (shape == "torus") {
                topology::TopologicalMesh mesh = topology::create_torus(level * 10);
                std::cout << "Visualizing torus resolution " << (level * 10) << "\n";
                visualizer.renderEulerCharacteristic(mesh);
            }
            visualizer.show();
        }
        
        else if (submode == "complex") {
            std::string func_type = (argc > 3) ? argv[3] : "euler";
            int resolution = (argc > 4) ? std::stoi(argv[4]) : 400;

            if (func_type == "euler") {
                auto euler_func = [](complex_analysis::Complex z) -> complex_analysis::Complex {
                    return complex_analysis::exp_taylor_adaptive(complex_analysis::Complex(0, 1) * z);
                };
                std::cout << "Visualizing Euler formula e^(iz) at " << resolution << "x" << resolution << "\n";
                visualizer.renderComplexFunction(euler_func, -3.14159, 3.14159, -3.14159, 3.14159, resolution);
            } else if (func_type == "riemann") {
                auto riemann_func = [](complex_analysis::Complex z) -> complex_analysis::Complex {
                    return complex_analysis::Complex(1, 0) / z;
                };
                std::cout << "Visualizing Riemann function 1/z at " << resolution << "x" << resolution << "\n";
                visualizer.renderRiemannSurface(riemann_func, -2, 2, -2, 2, resolution);
            }
            visualizer.show();
        }
        
        else if (submode == "primes" || submode == "number") {
            int limit = (argc > 3) ? std::stoi(argv[3]) : 1000;
            std::cout << "Visualizing prime distribution up to " << limit << "\n";
            visualizer.renderPrimeDistribution(limit);
            visualizer.show();
        }
#endif
    }
    
    else if (mode == "ultra") {
        std::cout << "\n+=======================================+\n";
        std::cout << "| ULTRA PRECISION NOT AVAILABLE         |\n";
        std::cout << "+=======================================+\n";
        std::cout << "Ultra precision mode is not implemented in this optimized version.\n";
        std::cout << "Use 'complex' mode for high precision testing.\n";
        return 0;
    }
    
    else {
        std::cout << "Error: Unknown mode '" << mode << "'\n";
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}