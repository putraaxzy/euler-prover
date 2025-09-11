#include "visualization.h"
#include "number_theory.h"
#include <iostream>
#include <vector>
#include <cmath>

/**
 * Prime Distribution Demo
 * This demo visualizes prime number distributions using various methods.
 */
void prime_distribution_demo() {
    std::cout << "Generating Prime Distribution visualization...\n";
    
    std::cout << "Select visualization method:\n";
    std::cout << "1. Ulam Spiral\n";
    std::cout << "2. Sacks Spiral\n";
    std::cout << "Enter your choice (1-2): ";
    
    int choice;
    std::cin >> choice;
    
    std::string method;
    switch (choice) {
        case 1: method = "ulam"; break;
        case 2: method = "sacks"; break;
        default: method = "ulam";
    }
    
    std::cout << "Maximum number to visualize (up to 10000): ";
    int maxNumber;
    std::cin >> maxNumber;
    maxNumber = std::min(std::max(100, maxNumber), 10000);
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Prime Distribution - " + method + " spiral";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.setBackgroundColor(0.05, 0.05, 0.1);
    visualizer.renderPrimeDistribution(maxNumber, method);
    visualizer.show();
}

/**
 * Modular Arithmetic Demo
 * This demo visualizes modular arithmetic operations.
 */
void modular_arithmetic_demo() {
    std::cout << "Generating Modular Arithmetic visualization...\n";
    
    std::cout << "Select operation to visualize:\n";
    std::cout << "1. Addition (clock arithmetic)\n";
    std::cout << "2. Multiplication (number theory patterns)\n";
    std::cout << "Enter your choice (1-2): ";
    
    int operationChoice;
    std::cin >> operationChoice;
    
    std::string operation;
    switch (operationChoice) {
        case 1: operation = "addition"; break;
        case 2: operation = "multiplication"; break;
        default: operation = "addition";
    }
    
    std::cout << "Modulus (2-50): ";
    int modulus;
    std::cin >> modulus;
    modulus = std::min(std::max(2, modulus), 50);
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Modular " + operation + " (mod " + std::to_string(modulus) + ")";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.setBackgroundColor(0.05, 0.05, 0.1);
    visualizer.renderModularArithmetic(modulus, operation);
    visualizer.show();
}

/**
 * Euler's Totient Function Demo
 * This demo visualizes Euler's Totient function φ(n).
 */
void totient_function_demo() {
    std::cout << "Generating Euler's Totient Function visualization...\n";
    
    std::cout << "Maximum n value to visualize (up to 200): ";
    int maxN;
    std::cin >> maxN;
    maxN = std::min(std::max(10, maxN), 200);
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Euler's Totient Function φ(n)";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.setBackgroundColor(0.05, 0.05, 0.1);
    visualizer.renderTotientFunction(maxN);
    visualizer.show();
}

int main() {
    std::cout << "Number Theory Visualization Demos\n";
    std::cout << "================================\n";
    std::cout << "1. Prime Number Distribution\n";
    std::cout << "2. Modular Arithmetic\n";
    std::cout << "3. Euler's Totient Function\n";
    std::cout << "Choose a demo (1-3): ";
    
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1: prime_distribution_demo(); break;
        case 2: modular_arithmetic_demo(); break;
        case 3: totient_function_demo(); break;
        default: std::cout << "Invalid choice!\n"; break;
    }
    
    return 0;
}
