#include "topology.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <chrono>

namespace topology {

double Vector3::norm() const { 
    return std::sqrt(x*x + y*y + z*z); 
}

Vector3 Vector3::normalized() const { 
    double n = norm(); 
    return n > 0 ? *this * (1.0/n) : *this; 
}

bool Vector3::operator==(const Vector3& v) const {
    constexpr double eps = 1e-12;
    return std::abs(x - v.x) < eps && std::abs(y - v.y) < eps && std::abs(z - v.z) < eps;
}

Edge::Edge(size_t a, size_t b) : vertices{std::min(a,b), std::max(a,b)} {}

bool Edge::operator<(const Edge& e) const {
    return vertices < e.vertices;
}

bool Edge::operator==(const Edge& e) const {
    return vertices == e.vertices;
}

std::tuple<size_t, size_t, size_t, int> TopologicalMesh::euler_characteristic() const {
    std::set<Edge> edges;
    
    for (const auto& face : faces) {
        edges.emplace(face.vertices[0], face.vertices[1]);
        edges.emplace(face.vertices[1], face.vertices[2]);
        edges.emplace(face.vertices[2], face.vertices[0]);
    }
    
    size_t V = vertices.size();
    size_t E = edges.size();
    size_t F = faces.size();
    int chi = static_cast<int>(V) - static_cast<int>(E) + static_cast<int>(F);
    
    return {V, E, F, chi};
}

bool TopologicalMesh::validate() const {
    if (vertices.empty() || faces.empty()) return false;
    
    for (const auto& face : faces) {
        for (size_t i = 0; i < 3; i++) {
            if (face.vertices[i] >= vertices.size()) return false;
        }
    }
    
    return true;
}

std::pair<double, double> TopologicalMesh::geometric_properties() const {
    double surface_area = 0.0;
    double volume = 0.0;
    
    for (const auto& face : faces) {
        Vector3 v0 = vertices[face.vertices[0]];
        Vector3 v1 = vertices[face.vertices[1]];
        Vector3 v2 = vertices[face.vertices[2]];
        
        Vector3 e1 = v1 + v0 * (-1.0);
        Vector3 e2 = v2 + v0 * (-1.0);
        
        Vector3 cross = {
            e1.y * e2.z - e1.z * e2.y,
            e1.z * e2.x - e1.x * e2.z,
            e1.x * e2.y - e1.y * e2.x
        };
        
        surface_area += cross.norm() * 0.5;
        
        volume += (v0.x * (v1.y * v2.z - v1.z * v2.y) +
                  v0.y * (v1.z * v2.x - v1.x * v2.z) +
                  v0.z * (v1.x * v2.y - v1.y * v2.x)) / 6.0;
    }
    
    return {surface_area, std::abs(volume)};
}

TopologicalMesh PlatonicSolids::tetrahedron() {
    TopologicalMesh mesh;
    double s = std::sqrt(2.0/3.0);
    mesh.vertices = {
        {s, 0, -1.0/std::sqrt(3.0)}, {-s/2.0, 0.5, -1.0/std::sqrt(3.0)},
        {-s/2.0, -0.5, -1.0/std::sqrt(3.0)}, {0, 0, std::sqrt(3.0)/3.0}
    };
    mesh.faces = {{0,1,2}, {0,3,1}, {1,3,2}, {2,3,0}};
    return mesh;
}

TopologicalMesh PlatonicSolids::cube() {
    TopologicalMesh mesh;
    mesh.vertices = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
    };
    
    std::vector<std::array<int,4>> quads = {
        {0,1,2,3}, {4,7,6,5}, {0,4,5,1}, {2,6,7,3}, {0,3,7,4}, {1,5,6,2}
    };
    
    for (auto quad : quads) {
        mesh.faces.emplace_back(quad[0], quad[1], quad[2]);
        mesh.faces.emplace_back(quad[0], quad[2], quad[3]);
    }
    return mesh;
}

TopologicalMesh PlatonicSolids::octahedron() {
    TopologicalMesh mesh;
    mesh.vertices = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1}
    };
    mesh.faces = {
        {0,2,4}, {2,1,4}, {1,3,4}, {3,0,4},
        {2,0,5}, {1,2,5}, {3,1,5}, {0,3,5}
    };
    return mesh;
}

TopologicalMesh PlatonicSolids::icosahedron() {
    TopologicalMesh mesh;
    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    const double a = 1.0;
    const double b = 1.0 / phi;
    
    mesh.vertices = {
        {0,b,-a}, {b,a,0}, {-b,a,0}, {0,b,a}, {0,-b,a}, {-a,0,b},
        {0,-b,-a}, {a,0,-b}, {a,0,b}, {-a,0,-b}, {b,-a,0}, {-b,-a,0}
    };
    
    mesh.faces = {
        {2,1,0}, {1,2,3}, {5,4,3}, {4,8,3}, {7,6,0}, {6,9,0},
        {11,10,4}, {10,11,6}, {9,5,2}, {5,9,11}, {8,7,1}, {7,8,10},
        {2,5,3}, {8,1,3}, {9,2,0}, {1,7,0}, {11,9,6}, {7,10,6},
        {5,11,4}, {10,8,4}
    };
    
    return mesh;
}

TopologicalMesh PlatonicSolids::dodecahedron() {
    TopologicalMesh mesh;
    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    const double inv_phi = 1.0 / phi;
    
    mesh.vertices = {
        {1,1,1}, {1,1,-1}, {1,-1,1}, {1,-1,-1},
        {-1,1,1}, {-1,1,-1}, {-1,-1,1}, {-1,-1,-1},
        {0,inv_phi,phi}, {0,inv_phi,-phi}, {0,-inv_phi,phi}, {0,-inv_phi,-phi},
        {inv_phi,phi,0}, {inv_phi,-phi,0}, {-inv_phi,phi,0}, {-inv_phi,-phi,0},
        {phi,0,inv_phi}, {phi,0,-inv_phi}, {-phi,0,inv_phi}, {-phi,0,-inv_phi}
    };
    
    mesh.faces = {
        {0,8,4}, {0,4,14}, {0,14,12}, {0,12,16}, {0,16,8},
        {3,9,1}, {3,1,17}, {3,17,13}, {3,13,11}, {3,11,9},
        {2,10,6}, {2,6,18}, {2,18,15}, {2,15,13}, {2,13,17},
        {7,11,5}, {7,5,19}, {7,19,15}, {7,15,6}, {7,6,10},
        {1,9,5}, {1,5,14}, {1,14,4}, {1,4,12}, {1,12,17},
        {8,10,2}, {8,2,16}, {16,2,17}, {17,12,14}, {14,5,19},
        {19,15,18}, {18,6,10}, {10,8,16}, {9,11,7}, {11,13,15},
        {13,17,1}, {4,8,10}, {5,9,7}, {6,11,3}, {7,19,5}
    };
    
    return mesh;
}

uint64_t IcosphereGenerator::edge_key(size_t v1, size_t v2) const {
    if (v1 > v2) std::swap(v1, v2);
    return (static_cast<uint64_t>(v1) << 32) | v2;
}

size_t IcosphereGenerator::get_midpoint(size_t v1, size_t v2) {
    uint64_t key = edge_key(v1, v2);
    auto it = midpoint_cache.find(key);
    
    if (it != midpoint_cache.end()) {
        return it->second;
    }
    
    Vector3 p1 = mesh.vertices[v1];
    Vector3 p2 = mesh.vertices[v2];
    Vector3 mid = (p1 + p2) * 0.5;
    mid = mid.normalized();
    
    size_t index = mesh.vertices.size();
    mesh.vertices.push_back(mid);
    midpoint_cache[key] = index;
    
    return index;
}

TopologicalMesh IcosphereGenerator::generate(int subdivision_level) {
    mesh = PlatonicSolids::icosahedron();
    
    for (auto& vertex : mesh.vertices) {
        vertex = vertex.normalized();
    }
    
    for (int level = 0; level < subdivision_level; level++) {
        std::vector<Triangle> new_faces;
        new_faces.reserve(mesh.faces.size() * 4);
        
        for (const auto& face : mesh.faces) {
            size_t v1 = face.vertices[0];
            size_t v2 = face.vertices[1];
            size_t v3 = face.vertices[2];
            
            size_t a = get_midpoint(v1, v2);
            size_t b = get_midpoint(v2, v3);
            size_t c = get_midpoint(v3, v1);
            
            new_faces.emplace_back(v1, a, c);
            new_faces.emplace_back(v2, b, a);
            new_faces.emplace_back(v3, c, b);
            new_faces.emplace_back(a, b, c);
        }
        
        mesh.faces = std::move(new_faces);
        midpoint_cache.clear();
    }
    
    return mesh;
}

void TopologyTestResult::add_test(const std::string& name, const TopologicalMesh& mesh, int expected_chi) {
    auto start = std::chrono::high_resolution_clock::now();
    
    TestCase test;
    test.name = name;
    test.expected_characteristic = expected_chi;
    
    auto [V, E, F, chi] = mesh.euler_characteristic();
    test.vertices = V;
    test.edges = E;
    test.faces = F;
    test.euler_characteristic = chi;
    test.passed = (chi == expected_chi);
    
    if (!test.passed) all_passed = false;
    
    auto [surface_area, volume] = mesh.geometric_properties();
    test.surface_area = surface_area;
    test.volume = volume;
    
    auto end = std::chrono::high_resolution_clock::now();
    test.computation_time = std::chrono::duration<double>(end - start).count();
    total_computation_time += test.computation_time;
    
    test_cases.push_back(test);
}

TopologyTestResult run_comprehensive_suite() {
    TopologyTestResult result;
    
    result.add_test("Tetrahedron", PlatonicSolids::tetrahedron());
    result.add_test("Cube", PlatonicSolids::cube());
    result.add_test("Octahedron", PlatonicSolids::octahedron());
    result.add_test("Dodecahedron", PlatonicSolids::dodecahedron());
    result.add_test("Icosahedron", PlatonicSolids::icosahedron());
    
    IcosphereGenerator generator;
    for (int level = 0; level <= 4; level++) {
        std::string name = "Icosphere L" + std::to_string(level);
        result.add_test(name, generator.generate(level));
    }
    
    return result;
}

}
