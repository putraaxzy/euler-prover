#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <array>
#include "topology.h"
#include "complex_analysis.h"
#include "number_theory.h"

namespace visualization {

    enum class VisualizationType {
        TOPOLOGY_RIEMANN_SURFACE,
        TOPOLOGY_EULER_CHARACTERISTIC,
        TOPOLOGY_MANIFOLD,
        TOPOLOGY_KNOT_THEORY,
        COMPLEX_FUNCTION_PLOT,
        COMPLEX_CONFORMAL_MAPPING,
        COMPLEX_EULER_FORMULA,
        NUMBER_THEORY_PRIME_DISTRIBUTION,
        NUMBER_THEORY_MODULAR_ARITHMETIC,
        NUMBER_THEORY_TOTIENT_FUNCTION
    };

    struct VisualizationConfig {
        int width = 800;
        int height = 600;
        std::string title = "Euler Prover Visualization";
        bool interactive = true;
        std::string outputFilePath = "";
        double rotationSpeed = 1.0;
        double zoomSpeed = 1.0;
        int quality = 1;
    };

    struct VisualizerImpl;
    
    class Visualizer3D {
    private:
        std::unique_ptr<VisualizerImpl> impl;
        VisualizationConfig config;
        
    public:
        Visualizer3D(const VisualizationConfig& cfg = {});
        ~Visualizer3D();
        
        Visualizer3D(const Visualizer3D&) = delete;
        Visualizer3D& operator=(const Visualizer3D&) = delete;
        
        Visualizer3D(Visualizer3D&&) noexcept;
        Visualizer3D& operator=(Visualizer3D&&) noexcept;
        
        void renderRiemannSurface(const std::function<complex_analysis::Complex(complex_analysis::Complex)>& function, 
                                double xMin, double xMax, double yMin, double yMax, int resolution = 100);
        
        void renderEulerCharacteristic(const topology::TopologicalMesh& mesh);
        
        void renderManifold(const std::vector<topology::Vector3>& points, 
                          const std::vector<std::vector<size_t>>& faces);
        
        void renderKnotTheory(const std::vector<topology::Vector3>& knotPoints, double tubeRadius = 0.1);
        
        void renderComplexFunction(const std::function<complex_analysis::Complex(complex_analysis::Complex)>& function, 
                                double xMin, double xMax, double yMin, double yMax, int resolution = 100);
        
        void renderConformalMapping(const std::function<complex_analysis::Complex(complex_analysis::Complex)>& function,
                                  const std::vector<std::vector<complex_analysis::Complex>>& gridLines, 
                                  int resolution = 100);
                                  
        void visualize_mesh(const topology::TopologicalMesh& mesh, VisualizationType type) {
            if (type == VisualizationType::TOPOLOGY_EULER_CHARACTERISTIC) {
                renderEulerCharacteristic(mesh);
            } else {
                std::vector<std::vector<size_t>> faces;
                for (const auto& face : mesh.faces) {
                    std::vector<size_t> vertices(face.vertices.begin(), face.vertices.end());
                    faces.push_back(vertices);
                }
                renderManifold(mesh.vertices, faces);
            }
            show();
        }
        
        void visualize_complex_function(
            const std::function<std::complex<double>(std::complex<double>)>& function,
            int resolution,
            VisualizationType type) {
                
            auto adapter = [function](complex_analysis::Complex z) -> complex_analysis::Complex {
                std::complex<double> input(static_cast<double>(z.real()), static_cast<double>(z.imag()));
                std::complex<double> result = function(input);
                return complex_analysis::Complex(result.real(), result.imag());
            };
                
            if (type == VisualizationType::COMPLEX_FUNCTION_PLOT) {
                renderComplexFunction(adapter, -5.0, 5.0, -5.0, 5.0, resolution);
            } else {
                renderRiemannSurface(adapter, -5.0, 5.0, -5.0, 5.0, resolution);
            }
            show();
        }
        
        void visualize_number_theory(VisualizationType type, int limit) {
            if (type == VisualizationType::NUMBER_THEORY_PRIME_DISTRIBUTION) {
                renderPrimeDistribution(limit);
            } else if (type == VisualizationType::NUMBER_THEORY_MODULAR_ARITHMETIC) {
                renderModularArithmetic(limit);
            } else if (type == VisualizationType::NUMBER_THEORY_TOTIENT_FUNCTION) {
                renderTotientFunction(limit);
            }
            show();
        }
        
        void renderEulerFormula(double theta, int revolutions = 1, int points = 100);
        void renderPrimeDistribution(int maxNumber, const std::string& method = "ulam");
        void renderModularArithmetic(int modulus, const std::string& operation = "addition");
        void renderTotientFunction(int maxN = 100);
        
        void show();
        void saveImage(const std::string& filePath);
        void setBackgroundColor(double r, double g, double b);
        void addLegend(const std::vector<std::pair<std::string, std::array<double, 3>>>& items);
        void addAxes(bool show = true);
        void setViewpoint(double azimuth, double elevation, double distance);
        void enableAnimation(bool enable = true);
        void setAnimationCallback(const std::function<void(double)>& callback);
        void setTitle(const std::string& title);
    };

}
