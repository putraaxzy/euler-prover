#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    std::cout << "==================================\n";
    std::cout << "  EULER PROVER VISUALIZATION TOOL  \n";
    std::cout << "==================================\n\n";
    
    std::cout << "Choose a visualization category:\n";
    std::cout << "1. Topology Visualizations\n";
    std::cout << "2. Complex Analysis Visualizations\n";
    std::cout << "3. Number Theory Visualizations\n";
    std::cout << "0. Exit\n\n";
    
    std::cout << "Enter your choice (0-3): ";
    
    int choice;
    std::cin >> choice;
    
    std::string command;
    
    switch (choice) {
        case 1:
            #ifdef _WIN32
                system("topology_demo.exe");
            #else
                system("./topology_demo");
            #endif
            break;
            
        case 2:
            #ifdef _WIN32
                system("complex_analysis_demo.exe");
            #else
                system("./complex_analysis_demo");
            #endif
            break;
            
        case 3:
            #ifdef _WIN32
                system("number_theory_demo.exe");
            #else
                system("./number_theory_demo");
            #endif
            break;
            
        case 0:
        default:
            std::cout << "Exiting visualization tool.\n";
            break;
    }
    
    return 0;
}
