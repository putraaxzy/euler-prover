#pragma once
#include <vector>
#include <array>
#include <set>
#include <tuple>
#include <string>
#include <unordered_map>
#include <cstdint>

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
}
