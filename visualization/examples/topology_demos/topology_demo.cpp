#include "visualization.h"
#include "topology.h"
#include <iostream>
#include <vector>
#include <cmath>

/**
 * Euler Characteristic Demo
 * This demo visualizes a polyhedron and calculates its Euler characteristic.
 * It demonstrates the formula V - E + F = 2 for simple polyhedra.
 */
void euler_characteristic_demo() {
    std::cout << "Generating Euler Characteristic visualization...\n";
    
    // Create an icosahedron (a Platonic solid with 12 vertices, 30 edges, and 20 triangular faces)
    topology::Polyhedron icosahedron;
    
    // Golden ratio for icosahedron construction
    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    
    // Add the 12 vertices of an icosahedron
    icosahedron.vertices = {
        {0, 1, phi}, {0, -1, phi}, {0, 1, -phi}, {0, -1, -phi},
        {1, phi, 0}, {-1, phi, 0}, {1, -phi, 0}, {-1, -phi, 0},
        {phi, 0, 1}, {-phi, 0, 1}, {phi, 0, -1}, {-phi, 0, -1}
    };
    
    // Normalize vertices to have unit distance from the origin
    for (auto& v : icosahedron.vertices) {
        double length = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
        v.x /= length;
        v.y /= length;
        v.z /= length;
    }
    
    // Add the 20 triangular faces
    icosahedron.faces = {
        // 5 faces around vertex 0
        {0, 8, 1}, {0, 1, 9}, {0, 9, 5}, {0, 5, 4}, {0, 4, 8},
        // 5 faces adjacent to the above 5 faces
        {1, 8, 6}, {9, 1, 7}, {5, 9, 11}, {4, 5, 2}, {8, 4, 10},
        // 5 faces around vertex 3
        {3, 6, 7}, {3, 7, 11}, {3, 11, 2}, {3, 2, 10}, {3, 10, 6},
        // 5 faces adjacent to the above 5 faces
        {6, 10, 8}, {7, 6, 1}, {11, 7, 9}, {2, 11, 5}, {10, 2, 4}
    };
    
    // Create the edges
    for (const auto& face : icosahedron.faces) {
        for (size_t i = 0; i < face.vertices.size(); ++i) {
            size_t j = (i + 1) % face.vertices.size(); // Next vertex in the face
            icosahedron.edges.push_back({face.vertices[i], face.vertices[j]});
        }
    }
    
    // Visualize the icosahedron
    visualization::VisualizationConfig config;
    config.title = "Euler Characteristic - Icosahedron";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.addAxes(true);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderEulerCharacteristic(icosahedron);
    visualizer.show();
}

/**
 * Riemann Surface Demo
 * This demo visualizes a Riemann surface for a simple complex function.
 */
void riemann_surface_demo() {
    std::cout << "Generating Riemann Surface visualization...\n";
    
    // Create a visualization for the complex function f(z) = sqrt(z)
    auto sqrtFunction = [](complex_analysis::Complex z) {
        return std::sqrt(z);
    };
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Riemann Surface - sqrt(z)";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.addAxes(true);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderRiemannSurface(sqrtFunction, -5.0, 5.0, -5.0, 5.0, 100);
    visualizer.show();
}

/**
 * Knot Theory Demo
 * This demo visualizes a simple trefoil knot.
 */
void knot_theory_demo() {
    std::cout << "Generating Knot Theory visualization...\n";
    
    // Create a trefoil knot
    const int numPoints = 500;
    std::vector<topology::Vector3> knotPoints;
    
    for (int i = 0; i < numPoints; ++i) {
        double t = i * 2.0 * M_PI / numPoints;
        
        // Parametric equations for a trefoil knot
        double x = sin(t) + 2 * sin(2 * t);
        double y = cos(t) - 2 * cos(2 * t);
        double z = -sin(3 * t);
        
        knotPoints.push_back({x, y, z});
    }
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Knot Theory - Trefoil Knot";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.addAxes(true);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderKnotTheory(knotPoints, 0.15);
    visualizer.show();
}

/**
 * Manifold Visualization Demo
 * This demo visualizes a simple manifold (a torus).
 */
void manifold_demo() {
    std::cout << "Generating Manifold visualization...\n";
    
    // Create a torus as a manifold
    const int uResolution = 30;
    const int vResolution = 30;
    const double R = 2.0; // Major radius
    const double r = 0.6; // Minor radius
    
    std::vector<topology::Vector3> torusPoints;
    std::vector<std::vector<size_t>> torusFaces;
    
    // Generate vertices
    for (int u = 0; u < uResolution; ++u) {
        double theta = u * 2.0 * M_PI / uResolution;
        for (int v = 0; v < vResolution; ++v) {
            double phi = v * 2.0 * M_PI / vResolution;
            
            double x = (R + r * cos(phi)) * cos(theta);
            double y = (R + r * cos(phi)) * sin(theta);
            double z = r * sin(phi);
            
            torusPoints.push_back({x, y, z});
        }
    }
    
    // Generate faces
    for (int u = 0; u < uResolution; ++u) {
        for (int v = 0; v < vResolution; ++v) {
            int u1 = (u + 1) % uResolution;
            int v1 = (v + 1) % vResolution;
            
            int p00 = u * vResolution + v;
            int p01 = u * vResolution + v1;
            int p10 = u1 * vResolution + v;
            int p11 = u1 * vResolution + v1;
            
            // Each quad face is made of two triangular faces
            std::vector<size_t> face1 = {p00, p01, p11};
            std::vector<size_t> face2 = {p00, p11, p10};
            
            torusFaces.push_back(face1);
            torusFaces.push_back(face2);
        }
    }
    
    // Create visualizer
    visualization::VisualizationConfig config;
    config.title = "Manifold - Torus";
    config.width = 1024;
    config.height = 768;
    
    visualization::Visualizer3D visualizer(config);
    visualizer.addAxes(true);
    visualizer.setBackgroundColor(0.15, 0.15, 0.2);
    visualizer.renderManifold(torusPoints, torusFaces);
    visualizer.show();
}

int main() {
    std::cout << "Topology Visualization Demos\n";
    std::cout << "============================\n";
    std::cout << "1. Euler Characteristic\n";
    std::cout << "2. Riemann Surface\n";
    std::cout << "3. Knot Theory\n";
    std::cout << "4. Manifold (Torus)\n";
    std::cout << "Choose a demo (1-4): ";
    
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1: euler_characteristic_demo(); break;
        case 2: riemann_surface_demo(); break;
        case 3: knot_theory_demo(); break;
        case 4: manifold_demo(); break;
        default: std::cout << "Invalid choice!\n"; break;
    }
    
    return 0;
}
