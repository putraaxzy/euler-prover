#pragma once
#include <vector>
#include <array>
#include <set>
#include <tuple>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cmath> // For sin, cos

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884L
#endif

namespace topology {
    struct Vector3 {
        double x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
        
        Vector3 operator+(const Vector3& v) const { return {x+v.x, y+v.y, z+v.z}; }
        Vector3 operator*(double s) const { return {x*s, y*s, z*s}; }
        double norm() const;
        Vector3 normalized() const;
        bool operator==(const Vector3& v) const;
    };
    
    struct Triangle {
        std::array<size_t, 3> vertices;
        Triangle(size_t a, size_t b, size_t c) : vertices{a, b, c} {}
    };
    
    struct Edge {
        std::array<size_t, 2> vertices;
        Edge(size_t a, size_t b);
        bool operator<(const Edge& e) const;
        bool operator==(const Edge& e) const;
    };
    
    class TopologicalMesh {
    public:
        std::vector<Vector3> vertices;
        std::vector<Triangle> faces;
        
        std::tuple<size_t, size_t, size_t, int> euler_characteristic() const;
        bool validate() const;
        std::pair<double, double> geometric_properties() const;
    };
    
    class PlatonicSolids {
    public:
        static TopologicalMesh tetrahedron();
        static TopologicalMesh cube();
        static TopologicalMesh octahedron();
        static TopologicalMesh icosahedron();
        static TopologicalMesh dodecahedron();
    };
    
    class IcosphereGenerator {
        std::unordered_map<uint64_t, size_t> midpoint_cache;
        TopologicalMesh mesh;
        
        uint64_t edge_key(size_t v1, size_t v2) const;
        size_t get_midpoint(size_t v1, size_t v2);
        
    public:
        TopologicalMesh generate(int subdivision_level);
    };
    
    struct TopologyTestResult {
        struct TestCase {
            std::string name;
            size_t vertices, edges, faces;
            int euler_characteristic;
            int expected_characteristic;
            bool passed;
            double surface_area, volume;
            double computation_time;
        };
        
        std::vector<TestCase> test_cases;
        bool all_passed = true;
        double total_computation_time = 0.0;
        
        void add_test(const std::string& name, const TopologicalMesh& mesh, int expected_chi = 2);
    };
    
    TopologyTestResult run_comprehensive_suite();

    // Additional mesh creation functions for visualization
    inline TopologicalMesh create_icosphere(int level) {
        IcosphereGenerator generator;
        return generator.generate(level);
    }

    inline TopologicalMesh create_torus(int resolution) {
        TopologicalMesh mesh;
        const double R = 1.0;  // major radius
        const double r = 0.3;  // minor radius
        
        // Generate vertices
        for (int i = 0; i < resolution; i++) {
            double phi = 2.0 * M_PI * i / resolution;
            for (int j = 0; j < resolution; j++) {
                double theta = 2.0 * M_PI * j / resolution;
                double x = (R + r * cos(theta)) * cos(phi);
                double y = (R + r * cos(theta)) * sin(phi);
                double z = r * sin(theta);
                mesh.vertices.push_back({x, y, z});
            }
        }
        
        // Generate triangles
        for (int i = 0; i < resolution; i++) {
            int ip = (i + 1) % resolution;
            for (int j = 0; j < resolution; j++) {
                int jp = (j + 1) % resolution;
                
                // First triangle
                mesh.faces.push_back({
                    static_cast<size_t>(i * resolution + j),
                    static_cast<size_t>(ip * resolution + j),
                    static_cast<size_t>(i * resolution + jp)
                });
                
                // Second triangle
                mesh.faces.push_back({
                    static_cast<size_t>(ip * resolution + j),
                    static_cast<size_t>(ip * resolution + jp),
                    static_cast<size_t>(i * resolution + jp)
                });
            }
        }
        
        return mesh;
    }
    
    inline TopologicalMesh create_klein_bottle(int resolution) {
        TopologicalMesh mesh;
        const double scale = 1.0;
        
        // Generate vertices
        for (int i = 0; i <= resolution; i++) {
            double u = i * 2.0 * M_PI / resolution;
            for (int j = 0; j <= resolution; j++) {
                double v = j * 2.0 * M_PI / resolution;
                
                // Parametric equations for Klein bottle
                double x = scale * (cos(u) * (3 + cos(u/2) * sin(v) - sin(u/2) * sin(2*v)));
                double y = scale * (sin(u) * (3 + cos(u/2) * sin(v) - sin(u/2) * sin(2*v)));
                double z = scale * (sin(u/2) * sin(v) + cos(u/2) * sin(2*v));
                
                mesh.vertices.push_back({x, y, z});
            }
        }
        
        // Generate triangles
        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                size_t idx = i * (resolution + 1) + j;
                size_t idx_right = idx + 1;
                size_t idx_below = idx + (resolution + 1);
                size_t idx_below_right = idx_below + 1;
                
                // First triangle
                mesh.faces.push_back({idx, idx_right, idx_below});
                
                // Second triangle
                mesh.faces.push_back({idx_below, idx_right, idx_below_right});
            }
        }
        
        return mesh;
    }
}
