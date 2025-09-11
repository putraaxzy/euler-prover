#include "visualization.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <memory>
#include <random>

#ifdef VTK_FOUND
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkPolyLine.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCubeAxesActor.h>
#include <vtkNamedColors.h>
#include <vtkDelaunay2D.h>
#include <vtkElevationFilter.h>
#include <vtkWarpScalar.h>
#include <vtkContourFilter.h>
#include <vtkStripper.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkPlaneSource.h>
#include <vtkRegularPolygonSource.h>
#include <vtkParametricTorus.h>
#include <vtkParametricKlein.h>
#include <vtkParametricMobius.h>
#include <vtkParametricBoy.h>
#include <vtkParametricDini.h>
#include <vtkParametricEnneper.h>
#include <vtkParametricFigure8Klein.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricRoman.h>
#include <vtkParametricSpline.h>
#include <vtkParametricSuperToroid.h>
#include <vtkMath.h>
#include <vtkVersion.h>
#endif

namespace visualization {

struct VisualizerImpl {
#ifdef VTK_FOUND
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> interactor;
    vtkSmartPointer<vtkNamedColors> colors;
    std::vector<vtkSmartPointer<vtkActor>> actors;
    vtkSmartPointer<vtkScalarBarActor> scalarBar;
    vtkSmartPointer<vtkOrientationMarkerWidget> orientationWidget;
    vtkSmartPointer<vtkAxesActor> axes;
    
    VisualizerImpl(int w, int h) {
        colors = vtkSmartPointer<vtkNamedColors>::New();
        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        
        renderWindow->AddRenderer(renderer);
        renderWindow->SetInteractor(interactor);
        renderWindow->SetSize(w, h);
        renderWindow->SetWindowName("Euler Mathematical Visualization");
        
        renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());
        
        setupLighting();
        setupCamera();
        setupInteraction();
        setupAxes();
    }
    
    void setupLighting() {
        renderer->GetLights()->RemoveAllItems();
        
        auto light1 = vtkSmartPointer<vtkLight>::New();
        light1->SetPosition(10, 10, 10);
        light1->SetFocalPoint(0, 0, 0);
        light1->SetColor(1.0, 1.0, 1.0);
        light1->SetIntensity(0.8);
        renderer->AddLight(light1);
        
        auto light2 = vtkSmartPointer<vtkLight>::New();
        light2->SetPosition(-10, -10, 5);
        light2->SetFocalPoint(0, 0, 0);
        light2->SetColor(0.8, 0.9, 1.0);
        light2->SetIntensity(0.4);
        renderer->AddLight(light2);
        
        auto light3 = vtkSmartPointer<vtkLight>::New();
        light3->SetPosition(0, 0, 15);
        light3->SetFocalPoint(0, 0, 0);
        light3->SetColor(1.0, 0.9, 0.8);
        light3->SetIntensity(0.3);
        renderer->AddLight(light3);
    }
    
    void setupCamera() {
        auto camera = renderer->GetActiveCamera();
        camera->SetPosition(8, 8, 8);
        camera->SetFocalPoint(0, 0, 0);
        camera->SetViewUp(0, 0, 1);
        camera->SetViewAngle(45);
        renderer->ResetCamera();
    }
    
    void setupInteraction() {
        auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        interactor->SetInteractorStyle(style);
    }
    
    void setupAxes() {
        axes = vtkSmartPointer<vtkAxesActor>::New();
        axes->SetTotalLength(2.0, 2.0, 2.0);
        axes->SetShaftType(vtkAxesActor::CYLINDER_SHAFT);
        axes->SetTipType(vtkAxesActor::CONE_TIP);
        
        orientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
        orientationWidget->SetOrientationMarker(axes);
        orientationWidget->SetInteractor(interactor);
        orientationWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
        orientationWidget->SetEnabled(1);
        orientationWidget->InteractiveOff();
    }
    
    void saveImage(const std::string& filename) {
        auto windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(renderWindow);
        windowToImageFilter->SetScale(2);
        windowToImageFilter->SetInputBufferTypeToRGBA();
        windowToImageFilter->ReadFrontBufferOff();
        windowToImageFilter->Update();
        
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == "png") {
            auto writer = vtkSmartPointer<vtkPNGWriter>::New();
            writer->SetFileName(filename.c_str());
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        } else if (ext == "jpg" || ext == "jpeg") {
            auto writer = vtkSmartPointer<vtkJPEGWriter>::New();
            writer->SetFileName(filename.c_str());
            writer->SetQuality(95);
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        } else if (ext == "bmp") {
            auto writer = vtkSmartPointer<vtkBMPWriter>::New();
            writer->SetFileName(filename.c_str());
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        } else if (ext == "tiff" || ext == "tif") {
            auto writer = vtkSmartPointer<vtkTIFFWriter>::New();
            writer->SetFileName(filename.c_str());
            writer->SetInputConnection(windowToImageFilter->GetOutputPort());
            writer->Write();
        } else {
            savePPM(filename);
        }
    }
    
    void addColorBar(const std::string& title, vtkSmartPointer<vtkLookupTable> lut) {
        scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
        scalarBar->SetLookupTable(lut);
        scalarBar->SetTitle(title.c_str());
        scalarBar->SetNumberOfLabels(5);
        scalarBar->SetPosition(0.85, 0.1);
        scalarBar->SetWidth(0.1);
        scalarBar->SetHeight(0.8);
        scalarBar->GetTitleTextProperty()->SetColor(1, 1, 1);
        scalarBar->GetLabelTextProperty()->SetColor(1, 1, 1);
        renderer->AddActor2D(scalarBar);
    }
    
    vtkSmartPointer<vtkPolyData> createHighQualitySurface(vtkSmartPointer<vtkPolyData> input) {
        auto cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
        cleaner->SetInputData(input);
        
        auto normals = vtkSmartPointer<vtkPolyDataNormals>::New();
        normals->SetInputConnection(cleaner->GetOutputPort());
        normals->ComputePointNormalsOn();
        normals->ComputeCellNormalsOn();
        normals->SplittingOff();
        
        auto smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
        smoother->SetInputConnection(normals->GetOutputPort());
        smoother->SetNumberOfIterations(50);
        smoother->SetRelaxationFactor(0.1);
        smoother->FeatureEdgeSmoothingOff();
        smoother->BoundarySmoothingOn();
        
        smoother->Update();
        return smoother->GetOutput();
    }
#else
    std::vector<std::vector<uint8_t>> imageData;
    int width, height;
    
    VisualizerImpl(int w, int h) : width(w), height(h) {
        imageData.resize(height, std::vector<uint8_t>(width * 3, 255));
    }
    
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            imageData[y][x * 3] = r;
            imageData[y][x * 3 + 1] = g;
            imageData[y][x * 3 + 2] = b;
        }
    }
#endif
    
    void savePPM(const std::string& filename) {
#ifndef VTK_FOUND
        std::ofstream file(filename);
        file << "P3\n" << width << " " << height << "\n255\n";
        for (const auto& row : imageData) {
            for (size_t i = 0; i < row.size(); i += 3) {
                file << (int)row[i] << " " << (int)row[i+1] << " " << (int)row[i+2] << " ";
            }
            file << "\n";
        }
        file.close();
#endif
    }
};

Visualizer3D::Visualizer3D(const VisualizationConfig& cfg) 
    : config(cfg), impl(std::make_unique<VisualizerImpl>(cfg.width, cfg.height)) {}

Visualizer3D::~Visualizer3D() = default;
Visualizer3D::Visualizer3D(Visualizer3D&&) noexcept = default;
Visualizer3D& Visualizer3D::operator=(Visualizer3D&&) noexcept = default;

void Visualizer3D::renderRiemannSurface(
    const std::function<complex_analysis::Complex(complex_analysis::Complex)>& function,
    double xMin, double xMax, double yMin, double yMax, int resolution) {
    
#ifdef VTK_FOUND
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto triangles = vtkSmartPointer<vtkCellArray>::New();
    auto colors = vtkSmartPointer<vtkFloatArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    
    std::vector<std::vector<vtkIdType>> pointIds(resolution, std::vector<vtkIdType>(resolution));
    
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            double x = xMin + (xMax - xMin) * i / (resolution - 1.0);
            double y = yMin + (yMax - yMin) * j / (resolution - 1.0);
            
            complex_analysis::Complex z(x, y);
            complex_analysis::Complex result = function(z);
            
            double magnitude = std::sqrt(result.real() * result.real() + result.imag() * result.imag());
            double phase = std::atan2(result.imag(), result.real());
            
            double height = std::log(1.0 + magnitude) * 0.5;
            height = std::min(height, 3.0);
            
            pointIds[i][j] = points->InsertNextPoint(x, y, height);
            
            double r = 0.5 + 0.5 * std::sin(phase);
            double g = 0.5 + 0.5 * std::cos(phase);
            double b = std::min(1.0, magnitude / 2.0);
            colors->InsertNextTuple3(r, g, b);
        }
    }
    
    for (int i = 0; i < resolution - 1; ++i) {
        for (int j = 0; j < resolution - 1; ++j) {
            auto triangle1 = vtkSmartPointer<vtkTriangle>::New();
            triangle1->GetPointIds()->SetId(0, pointIds[i][j]);
            triangle1->GetPointIds()->SetId(1, pointIds[i+1][j]);
            triangle1->GetPointIds()->SetId(2, pointIds[i][j+1]);
            triangles->InsertNextCell(triangle1);
            
            auto triangle2 = vtkSmartPointer<vtkTriangle>::New();
            triangle2->GetPointIds()->SetId(0, pointIds[i+1][j]);
            triangle2->GetPointIds()->SetId(1, pointIds[i+1][j+1]);
            triangle2->GetPointIds()->SetId(2, pointIds[i][j+1]);
            triangles->InsertNextCell(triangle2);
        }
    }
    
    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetPolys(triangles);
    polyData->GetPointData()->SetScalars(colors);
    
    auto smoothedData = impl->createHighQualitySurface(polyData);
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(smoothedData);
    mapper->ScalarVisibilityOn();
    
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToPhong();
    actor->GetProperty()->SetSpecular(0.3);
    actor->GetProperty()->SetSpecularPower(30);
    actor->GetProperty()->SetAmbient(0.2);
    actor->GetProperty()->SetDiffuse(0.8);
    
    impl->renderer->AddActor(actor);
    impl->actors.push_back(actor);
    
    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("Riemann Surface Visualization");
    textActor->GetTextProperty()->SetFontSize(18);
    textActor->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
    textActor->SetPosition(10, 10);
    impl->renderer->AddActor2D(textActor);
#else
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            double x = xMin + (xMax - xMin) * i / resolution;
            double y = yMin + (yMax - yMin) * j / resolution;
            
            complex_analysis::Complex z(x, y);
            complex_analysis::Complex result = function(z);
            
            double magnitude = std::sqrt(result.real() * result.real() + result.imag() * result.imag());
            double phase = std::atan2(result.imag(), result.real());
            
            uint8_t r = (uint8_t)(128 + 127 * std::sin(phase));
            uint8_t g = (uint8_t)(128 + 127 * std::cos(phase));
            uint8_t b = (uint8_t)(255 * std::min(1.0, magnitude / 2.0));
            
            int px = (int)(impl->width * i / resolution);
            int py = (int)(impl->height * j / resolution);
            impl->setPixel(px, py, r, g, b);
        }
    }
#endif
}

void Visualizer3D::renderEulerCharacteristic(const topology::TopologicalMesh& mesh) {
    auto [V, E, F, euler_char] = mesh.euler_characteristic();
    std::cout << "[VISUALIZATION] Euler characteristic V=" << V 
              << " E=" << E << " F=" << F << " χ=" << euler_char << std::endl;
    
#ifdef VTK_FOUND
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto triangles = vtkSmartPointer<vtkCellArray>::New();
    auto colors = vtkSmartPointer<vtkFloatArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    
    for (const auto& vertex : mesh.vertices) {
        points->InsertNextPoint(vertex.x, vertex.y, vertex.z);
        
        double hue = std::atan2(vertex.y, vertex.x) / (2.0 * M_PI) + 0.5;
        double sat = std::min(1.0, std::sqrt(vertex.x*vertex.x + vertex.y*vertex.y) / 2.0);
        double val = 0.8 + 0.2 * std::sin(vertex.z * 3.0);
        
        double r, g, b;
        if (sat == 0) {
            r = g = b = val;
        } else {
            double h = hue * 6.0;
            int i = (int)h;
            double f = h - i;
            double p = val * (1.0 - sat);
            double q = val * (1.0 - sat * f);
            double t = val * (1.0 - sat * (1.0 - f));
            
            switch (i % 6) {
                case 0: r = val; g = t; b = p; break;
                case 1: r = q; g = val; b = p; break;
                case 2: r = p; g = val; b = t; break;
                case 3: r = p; g = q; b = val; break;
                case 4: r = t; g = p; b = val; break;
                case 5: r = val; g = p; b = q; break;
            }
        }
        colors->InsertNextTuple3(r, g, b);
    }
    
    for (const auto& face : mesh.faces) {
        if (face.vertices.size() >= 3) {
            auto triangle = vtkSmartPointer<vtkTriangle>::New();
            triangle->GetPointIds()->SetId(0, face.vertices[0]);
            triangle->GetPointIds()->SetId(1, face.vertices[1]);
            triangle->GetPointIds()->SetId(2, face.vertices[2]);
            triangles->InsertNextCell(triangle);
        }
    }
    
    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetPolys(triangles);
    polyData->GetPointData()->SetScalars(colors);
    
    auto smoothedData = impl->createHighQualitySurface(polyData);
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(smoothedData);
    mapper->ScalarVisibilityOn();
    
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToPhong();
    actor->GetProperty()->SetSpecular(0.4);
    actor->GetProperty()->SetSpecularPower(50);
    actor->GetProperty()->SetAmbient(0.1);
    actor->GetProperty()->SetDiffuse(0.9);
    
    impl->renderer->AddActor(actor);
    impl->actors.push_back(actor);
    
    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    std::string text = "Euler Characteristic: V=" + std::to_string(V) + 
                      " E=" + std::to_string(E) + " F=" + std::to_string(F) + 
                      " χ=" + std::to_string(euler_char);
    textActor->SetInput(text.c_str());
    textActor->GetTextProperty()->SetFontSize(16);
    textActor->GetTextProperty()->SetColor(1.0, 1.0, 0.0);
    textActor->SetPosition(10, 50);
    impl->renderer->AddActor2D(textActor);
#else
    for (size_t i = 0; i < mesh.vertices.size() && i < 1000; ++i) {
        int x = (int)(impl->width * (mesh.vertices[i].x + 2) / 4);
        int y = (int)(impl->height * (mesh.vertices[i].y + 2) / 4);
        impl->setPixel(x, y, 255, 0, 0);
    }
#endif
}

void Visualizer3D::renderManifold(const std::vector<topology::Vector3>& vertices,
                                  const std::vector<std::vector<size_t>>& faces) {
    std::cout << "[VISUALIZATION] Rendering manifold with " << vertices.size() << " vertices" << std::endl;
    
    for (const auto& face : faces) {
        if (face.size() >= 3) {
            for (size_t i = 0; i < face.size(); ++i) {
                size_t idx = face[i];
                if (idx < vertices.size()) {
                    int x = (int)(impl->width * (vertices[idx].x + 2) / 4);
                    int y = (int)(impl->height * (vertices[idx].y + 2) / 4);
                    impl->setPixel(x, y, 0, 255, 0);
                }
            }
        }
    }
}

void Visualizer3D::renderKnotTheory(const std::vector<topology::Vector3>& knotPoints, double) {
    std::cout << "[VISUALIZATION] Rendering knot with " << knotPoints.size() << " points" << std::endl;
    
    for (const auto& point : knotPoints) {
        int x = (int)(impl->width * (point.x + 2) / 4);
        int y = (int)(impl->height * (point.y + 2) / 4);
        impl->setPixel(x, y, 0, 0, 255);
    }
}

void Visualizer3D::renderComplexFunction(
    const std::function<complex_analysis::Complex(complex_analysis::Complex)>& function,
    double xMin, double xMax, double yMin, double yMax, int resolution) {
    renderRiemannSurface(function, xMin, xMax, yMin, yMax, resolution);
}

void Visualizer3D::renderConformalMapping(
    const std::function<complex_analysis::Complex(complex_analysis::Complex)>& function,
    const std::vector<std::vector<complex_analysis::Complex>>& gridLines, int) {
    
    std::cout << "[VISUALIZATION] Rendering conformal mapping" << std::endl;
    
    for (const auto& line : gridLines) {
        for (const auto& point : line) {
            complex_analysis::Complex result = function(point);
            int x = (int)(impl->width * (result.real() + 2) / 4);
            int y = (int)(impl->height * (result.imag() + 2) / 4);
            impl->setPixel(x, y, 255, 255, 0);
        }
    }
}

void Visualizer3D::renderEulerFormula(double theta, int revolutions, int points) {
    std::cout << "[VISUALIZATION] Rendering Euler formula e^(i*theta)" << std::endl;
    
#ifdef VTK_FOUND
    auto circlePoints = vtkSmartPointer<vtkPoints>::New();
    auto lines = vtkSmartPointer<vtkCellArray>::New();
    auto colors = vtkSmartPointer<vtkFloatArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    
    auto polyLine = vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(points);
    
    for (int i = 0; i < points; ++i) {
        double t = theta * revolutions * i / (points - 1.0);
        double x = std::cos(t);
        double y = std::sin(t);
        double z = t * 0.1;
        
        vtkIdType id = circlePoints->InsertNextPoint(x, y, z);
        polyLine->GetPointIds()->SetId(i, id);
        
        double hue = (double)i / points;
        double r = 0.5 + 0.5 * std::sin(hue * 6.28);
        double g = 0.5 + 0.5 * std::sin(hue * 6.28 + 2.09);
        double b = 0.5 + 0.5 * std::sin(hue * 6.28 + 4.18);
        colors->InsertNextTuple3(r, g, b);
    }
    
    lines->InsertNextCell(polyLine);
    
    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(circlePoints);
    polyData->SetLines(lines);
    polyData->GetPointData()->SetScalars(colors);
    
    auto tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
    tubeFilter->SetInputData(polyData);
    tubeFilter->SetRadius(0.02);
    tubeFilter->SetNumberOfSides(12);
    tubeFilter->CappingOn();
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(tubeFilter->GetOutputPort());
    mapper->ScalarVisibilityOn();
    
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToPhong();
    actor->GetProperty()->SetSpecular(0.6);
    actor->GetProperty()->SetSpecularPower(80);
    
    impl->renderer->AddActor(actor);
    impl->actors.push_back(actor);
    
    auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(0.05);
    sphereSource->SetThetaResolution(20);
    sphereSource->SetPhiResolution(20);
    
    auto sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    
    auto sphereActor = vtkSmartPointer<vtkActor>::New();
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    sphereActor->GetProperty()->SetSpecular(0.8);
    sphereActor->GetProperty()->SetSpecularPower(100);
    sphereActor->SetPosition(1.0, 0.0, 0.0);
    
    impl->renderer->AddActor(sphereActor);
    impl->actors.push_back(sphereActor);
    
    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("Euler Formula: e^(iθ) = cos θ + i sin θ");
    textActor->GetTextProperty()->SetFontSize(16);
    textActor->GetTextProperty()->SetColor(0.0, 1.0, 1.0);
    textActor->SetPosition(10, 90);
    impl->renderer->AddActor2D(textActor);
#else
    for (int i = 0; i < points; ++i) {
        double t = theta * revolutions * i / points;
        double x = std::cos(t);
        double y = std::sin(t);
        
        int px = (int)(impl->width * (x + 1) / 2);
        int py = (int)(impl->height * (y + 1) / 2);
        impl->setPixel(px, py, 255, 128, 0);
    }
#endif
}

void Visualizer3D::renderPrimeDistribution(int maxNumber, const std::string& method) {
    std::cout << "[VISUALIZATION] Rendering prime distribution up to " << maxNumber << std::endl;
    
    std::vector<bool> isPrime(maxNumber + 1, true);
    isPrime[0] = isPrime[1] = false;
    
    for (int i = 2; i * i <= maxNumber; ++i) {
        if (isPrime[i]) {
            for (int j = i * i; j <= maxNumber; j += i) {
                isPrime[j] = false;
            }
        }
    }
    
#ifdef VTK_FOUND
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto vertices = vtkSmartPointer<vtkCellArray>::New();
    auto colors = vtkSmartPointer<vtkFloatArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");
    
    std::vector<int> primes;
    for (int i = 2; i <= maxNumber; ++i) {
        if (isPrime[i]) {
            primes.push_back(i);
        }
    }
    
    if (method == "ulam") {
        int gridSize = (int)std::ceil(std::sqrt(maxNumber));
        int centerX = gridSize / 2;
        int centerY = gridSize / 2;
        
        std::vector<std::vector<int>> grid(gridSize, std::vector<int>(gridSize, 0));
        
        int x = centerX, y = centerY;
        int dx = 0, dy = -1;
        
        for (int i = 1; i <= maxNumber; ++i) {
            if (x >= 0 && x < gridSize && y >= 0 && y < gridSize) {
                grid[y][x] = i;
                
                if (isPrime[i]) {
                    double px = (x - centerX) * 0.1;
                    double py = (y - centerY) * 0.1;
                    double pz = std::log(i) * 0.05;
                    
                    vtkIdType id = points->InsertNextPoint(px, py, pz);
                    vertices->InsertNextCell(1, &id);
                    
                    double intensity = std::min(1.0, std::log(i) / std::log(maxNumber));
                    colors->InsertNextTuple3(intensity, 0.2, 1.0 - intensity);
                }
            }
            
            if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y)) {
                int temp = dx;
                dx = -dy;
                dy = temp;
            }
            x += dx;
            y += dy;
        }
    } else {
        for (size_t i = 0; i < primes.size(); ++i) {
            double angle = 2.0 * M_PI * i / primes.size();
            double radius = std::log(primes[i]) * 0.3;
            
            double px = radius * std::cos(angle);
            double py = radius * std::sin(angle);
            double pz = std::sqrt(primes[i]) * 0.01;
            
            vtkIdType id = points->InsertNextPoint(px, py, pz);
            vertices->InsertNextCell(1, &id);
            
            double hue = (double)primes[i] / maxNumber;
            colors->InsertNextTuple3(hue, 0.8, 1.0 - hue * 0.5);
        }
    }
    
    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetVerts(vertices);
    polyData->GetPointData()->SetScalars(colors);
    
    auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(0.02);
    sphereSource->SetThetaResolution(12);
    sphereSource->SetPhiResolution(12);
    
    auto glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputData(polyData);
    glyph->SetSourceConnection(sphereSource->GetOutputPort());
    glyph->ScalingOff();
    glyph->OrientOff();
    
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph->GetOutputPort());
    mapper->ScalarVisibilityOn();
    
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetSpecular(0.5);
    actor->GetProperty()->SetSpecularPower(30);
    
    impl->renderer->AddActor(actor);
    impl->actors.push_back(actor);
    
    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    std::string text = "Prime Distribution (" + method + "): " + std::to_string(primes.size()) + " primes up to " + std::to_string(maxNumber);
    textActor->SetInput(text.c_str());
    textActor->GetTextProperty()->SetFontSize(14);
    textActor->GetTextProperty()->SetColor(1.0, 0.8, 0.0);
    textActor->SetPosition(10, 130);
    impl->renderer->AddActor2D(textActor);
#else
    for (int i = 2; i <= maxNumber; ++i) {
        if (isPrime[i]) {
            int x = (i * impl->width) / maxNumber;
            int y = impl->height / 2;
            impl->setPixel(x, y, 255, 0, 255);
        }
    }
#endif
}

void Visualizer3D::renderModularArithmetic(int modulus, const std::string&) {
    std::cout << "[VISUALIZATION] Rendering modular arithmetic mod " << modulus << std::endl;
    
    for (int i = 0; i < modulus; ++i) {
        for (int j = 0; j < modulus; ++j) {
            int result = (i + j) % modulus;
            int x = (i * impl->width) / modulus;
            int y = (j * impl->height) / modulus;
            uint8_t color = (uint8_t)(255 * result / modulus);
            impl->setPixel(x, y, color, color, 255);
        }
    }
}

void Visualizer3D::renderTotientFunction(int maxN) {
    std::cout << "[VISUALIZATION] Rendering totient function up to " << maxN << std::endl;
    
    for (int n = 1; n <= maxN && n < impl->width; ++n) {
        int totient = 0;
        for (int i = 1; i <= n; ++i) {
            int gcd = 1;
            int a = i, b = n;
            while (b != 0) {
                int temp = b;
                b = a % b;
                a = temp;
            }
            gcd = a;
            if (gcd == 1) totient++;
        }
        
        int x = n;
        int y = impl->height - (totient * impl->height) / maxN;
        impl->setPixel(x, y, 0, 255, 255);
    }
}

void Visualizer3D::show() {
#ifdef VTK_FOUND
    impl->renderWindow->Render();
    if (config.interactive) {
        impl->interactor->Start();
    }
    
    if (!config.outputFilePath.empty()) {
        impl->saveImage(config.outputFilePath);
        std::cout << "[VISUALIZATION] Saved as: " << config.outputFilePath << std::endl;
    }
#else
    std::string filename = config.outputFilePath.empty() ? "euler_result.ppm" : config.outputFilePath;
    impl->savePPM(filename);
    std::cout << "[VISUALIZATION] Saved as: " << filename << std::endl;
#endif
}

void Visualizer3D::saveImage(const std::string& filePath) {
#ifdef VTK_FOUND
    impl->saveImage(filePath);
#else
    impl->savePPM(filePath);
#endif
}

void Visualizer3D::setBackgroundColor(double r, double g, double b) {
#ifdef VTK_FOUND
    impl->renderer->SetBackground(r, g, b);
#else
    uint8_t red = (uint8_t)(255 * r);
    uint8_t green = (uint8_t)(255 * g);
    uint8_t blue = (uint8_t)(255 * b);
    
    for (auto& row : impl->imageData) {
        for (size_t i = 0; i < row.size(); i += 3) {
            row[i] = red;
            row[i + 1] = green;
            row[i + 2] = blue;
        }
    }
#endif
}

void Visualizer3D::addLegend(const std::vector<std::pair<std::string, std::array<double, 3>>>& items) {
    std::cout << "[VISUALIZATION] Legend: ";
    for (const auto& item : items) {
        std::cout << item.first << " ";
    }
    std::cout << std::endl;
}

void Visualizer3D::addAxes(bool show) {
    if (show) {
        for (int i = 0; i < impl->width; ++i) {
            impl->setPixel(i, impl->height / 2, 128, 128, 128);
        }
        for (int j = 0; j < impl->height; ++j) {
            impl->setPixel(impl->width / 2, j, 128, 128, 128);
        }
    }
}

void Visualizer3D::setViewpoint(double azimuth, double elevation, double distance) {
#ifdef VTK_FOUND
    auto camera = impl->renderer->GetActiveCamera();
    
    double rad_az = azimuth * M_PI / 180.0;
    double rad_el = elevation * M_PI / 180.0;
    
    double x = distance * std::cos(rad_el) * std::cos(rad_az);
    double y = distance * std::cos(rad_el) * std::sin(rad_az);
    double z = distance * std::sin(rad_el);
    
    camera->SetPosition(x, y, z);
    camera->SetFocalPoint(0, 0, 0);
    camera->SetViewUp(0, 0, 1);
    impl->renderer->ResetCamera();
#endif
    std::cout << "[VISUALIZATION] Viewpoint: az=" << azimuth << " el=" << elevation << " dist=" << distance << std::endl;
}

void Visualizer3D::enableAnimation(bool enable) {
    std::cout << "[VISUALIZATION] Animation " << (enable ? "enabled" : "disabled") << std::endl;
}

void Visualizer3D::setAnimationCallback(const std::function<void(double)>&) {
    std::cout << "[VISUALIZATION] Animation callback set" << std::endl;
}

void Visualizer3D::setTitle(const std::string& title) {
    std::cout << "[VISUALIZATION] Title: " << title << std::endl;
}

}
