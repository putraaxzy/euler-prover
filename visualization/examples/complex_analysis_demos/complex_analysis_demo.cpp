#include "visualization.h"
#include "complex_analysis.h"
#include <iostream>
#include <vector>
#include <cmath>

/**
 * Complex Function Visualization Demo
 * This demo creates domain coloring visualizations for various complex functions.
 */
void complex_function_demo() {
    std::cout << "Generating Complex Function visualization...\n";
    
    std::cout << "Select a function to visualize:\n";
    std::cout << "1. f(z) = z^2\n";
    std::cout << "2. f(z) = 1/z\n";
    std::cout << "3. f(z) = e^z\n";
    std::cout << "4. f(z) = sin(z)\n";
    std::cout << "Enter your choice (1-4): ";
    
    int choice;
    std::cin >> choice;
    
    std::function<complex_analysis::Complex(complex_analysis::Complex)> function;
    std::string functionName;
    
    switch (choice) {
        case 1:
            function = [](complex_analysis::Complex z) { return z * z; };
            functionName = "f(z) = z^2";
            break;
        case 2:
            function = [](complex_analysis::Complex z) { 
                if (std::abs(z) < 1e-10) return complex_analysis::Complex(1e10, 0); 
                return complex_analysis::Complex(1.0) / z; 
            };
            functionName = "f(z) = 1/z";
            break;
        case 3:
            function = [](complex_analysis::Complex z) { return std::exp(z); };
            functionName = "f(z) = e^z";
            break;
        case 4:
            function = [](complex_analysis::Complex z) { return std::sin(z); };
            functionName = "f(z) = sin(z)";
            break;
        default:
            function = [](complex_analysis::Complex z) { return z; };
            functionName = "f(z) = z";
    }
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Complex Function - " + functionName;
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderComplexFunction(function, -5.0, 5.0, -5.0, 5.0, 200);
    visualizer.show();
}

/**
 * Conformal Mapping Demo
 * This demo visualizes conformal mappings between complex domains.
 */
void conformal_mapping_demo() {
    std::cout << "Generating Conformal Mapping visualization...\n";
    
    std::cout << "Select a conformal mapping to visualize:\n";
    std::cout << "1. f(z) = z^2 (Square mapping)\n";
    std::cout << "2. f(z) = e^z (Exponential mapping)\n";
    std::cout << "3. f(z) = (z-1)/(z+1) (Möbius transformation)\n";
    std::cout << "Enter your choice (1-3): ";
    
    int choice;
    std::cin >> choice;
    
    std::function<complex_analysis::Complex(complex_analysis::Complex)> function;
    std::string mappingName;
    
    switch (choice) {
        case 1:
            function = [](complex_analysis::Complex z) { return z * z; };
            mappingName = "f(z) = z^2";
            break;
        case 2:
            function = [](complex_analysis::Complex z) { return std::exp(z); };
            mappingName = "f(z) = e^z";
            break;
        case 3:
            function = [](complex_analysis::Complex z) { 
                return (z - complex_analysis::Complex(1.0)) / (z + complex_analysis::Complex(1.0)); 
            };
            mappingName = "f(z) = (z-1)/(z+1)";
            break;
        default:
            function = [](complex_analysis::Complex z) { return z; };
            mappingName = "f(z) = z";
    }
    
    // Create a grid in the complex plane
    std::vector<std::vector<complex_analysis::Complex>> gridLines;
    
    // Create horizontal grid lines
    for (int i = -5; i <= 5; i++) {
        std::vector<complex_analysis::Complex> line;
        for (int j = -500; j <= 500; j++) {
            double x = j * 0.01;
            line.push_back(complex_analysis::Complex(x, i));
        }
        gridLines.push_back(line);
    }
    
    // Create vertical grid lines
    for (int i = -5; i <= 5; i++) {
        std::vector<complex_analysis::Complex> line;
        for (int j = -500; j <= 500; j++) {
            double y = j * 0.01;
            line.push_back(complex_analysis::Complex(i, y));
        }
        gridLines.push_back(line);
    }
    
    // Create unit circle grid
    std::vector<complex_analysis::Complex> unitCircle;
    for (int i = 0; i <= 360; i++) {
        double angle = i * M_PI / 180.0;
        unitCircle.push_back(complex_analysis::Complex(cos(angle), sin(angle)));
    }
    gridLines.push_back(unitCircle);
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Conformal Mapping - " + mappingName;
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderConformalMapping(function, gridLines);
    visualizer.show();
}

/**
 * Euler's Formula Demo
 * This demo visualizes Euler's formula: e^(iθ) = cos(θ) + i*sin(θ)
 */
void euler_formula_demo() {
    std::cout << "Generating Euler's Formula visualization...\n";
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Euler's Formula - e^(iθ) = cos(θ) + i*sin(θ)";
    config.width = 1024;
    config.height = 768;
    
    std::cout << "Number of revolutions to show (1-10): ";
    int revolutions;
    std::cin >> revolutions;
    revolutions = std::min(std::max(1, revolutions), 10);
    
    double theta = revolutions * 2.0 * M_PI;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderEulerFormula(theta, revolutions, 500);
    visualizer.show();
}

int main() {
    std::cout << "Complex Analysis Visualization Demos\n";
    std::cout << "===================================\n";
    std::cout << "1. Complex Function Domain Coloring\n";
    std::cout << "2. Conformal Mapping\n";
    std::cout << "3. Euler's Formula\n";
    std::cout << "Choose a demo (1-3): ";
    
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1: complex_function_demo(); break;
        case 2: conformal_mapping_demo(); break;
        case 3: euler_formula_demo(); break;
        default: std::cout << "Invalid choice!\n"; break;
    }
    
    return 0;
}
