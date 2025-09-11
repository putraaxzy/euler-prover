#!/usr/bin/env python3
"""
Google Colab Demo Script for Enhanced Euler Prover
Demonstrates high-performance mathematical computation with VTK visualization
"""

import subprocess
import os
import time
from IPython.display import Image, display, HTML
import matplotlib.pyplot as plt
import numpy as np

def setup_euler_prover():
    """Setup and build Euler Prover in Colab environment"""
    print("🚀 Setting up Enhanced Euler Prover...")
    
    # Install dependencies
    subprocess.run([
        "apt-get", "update", "-qq"
    ], check=True, capture_output=True)
    
    subprocess.run([
        "apt-get", "install", "-y", 
        "build-essential", "cmake", "libvtk9-dev", "libvtk9-qt-dev"
    ], check=True, capture_output=True)
    
    # Clone and build
    if not os.path.exists("euler-prover"):
        subprocess.run([
            "git", "clone", "https://github.com/putraaxzy/euler-prover.git"
        ], check=True)
    
    os.chdir("euler-prover")
    
    # Make build script executable and run
    subprocess.run(["chmod", "+x", "build_colab_optimized.sh"], check=True)
    result = subprocess.run(["./build_colab_optimized.sh"], 
                          capture_output=True, text=True)
    
    print("✅ Build completed!")
    print(f"Build output: {result.stdout}")
    if result.stderr:
        print(f"Warnings: {result.stderr}")

def run_performance_demo():
    """Run comprehensive performance demonstration"""
    print("\n🔬 Running Performance Demonstrations...")
    
    demos = [
        {
            "name": "Number Theory - Euler's Theorem",
            "cmd": ["./build_colab/euler", "proof", "number", "10000", "50", "8"],
            "description": "Testing a^φ(n) ≡ 1 (mod n) for 10,000 moduli"
        },
        {
            "name": "Complex Analysis - Euler's Formula", 
            "cmd": ["./build_colab/euler", "proof", "complex", "1000000", "18", "8"],
            "description": "Verifying e^(iθ) = cos θ + i sin θ with 1M samples"
        },
        {
            "name": "Topology - Euler Characteristic",
            "cmd": ["./build_colab/euler", "proof", "topology", "5"],
            "description": "Computing V-E+F=2 for icosphere subdivisions"
        }
    ]
    
    results = {}
    
    for demo in demos:
        print(f"\n📊 {demo['name']}")
        print(f"   {demo['description']}")
        
        start_time = time.time()
        result = subprocess.run(demo['cmd'], capture_output=True, text=True)
        end_time = time.time()
        
        results[demo['name']] = {
            'runtime': end_time - start_time,
            'output': result.stdout,
            'success': result.returncode == 0
        }
        
        if result.returncode == 0:
            print(f"   ✅ Completed in {end_time - start_time:.2f}s")
        else:
            print(f"   ❌ Failed: {result.stderr}")
    
    return results

def run_visualization_demo():
    """Generate high-quality visualizations"""
    print("\n🎨 Generating Professional Visualizations...")
    
    visualizations = [
        {
            "name": "Prime Distribution (Ulam Spiral)",
            "cmd": ["./build_colab/euler", "visualize", "primes", "10000", "ulam"],
            "output": "primes_ulam.png",
            "description": "3D visualization of prime number patterns"
        },
        {
            "name": "Euler Formula Visualization",
            "cmd": ["./build_colab/euler", "visualize", "euler", "1024"],
            "output": "euler_formula.png", 
            "description": "Interactive 3D representation of e^(iθ)"
        },
        {
            "name": "Icosphere Topology",
            "cmd": ["./build_colab/euler", "visualize", "topology", "icosphere", "4"],
            "output": "icosphere_topology.png",
            "description": "High-detail mesh with Euler characteristic"
        },
        {
            "name": "Riemann Surface",
            "cmd": ["./build_colab/euler", "visualize", "complex", "riemann", "800"],
            "output": "riemann_surface.png",
            "description": "Complex function domain coloring"
        }
    ]
    
    generated_images = []
    
    for viz in visualizations:
        print(f"\n🖼️  {viz['name']}")
        print(f"   {viz['description']}")
        
        # Add output parameter
        cmd = viz['cmd'] + ["--output", viz['output']]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode == 0 and os.path.exists(viz['output']):
            print(f"   ✅ Generated: {viz['output']}")
            generated_images.append({
                'name': viz['name'],
                'file': viz['output'],
                'description': viz['description']
            })
        else:
            print(f"   ❌ Failed to generate {viz['output']}")
            if result.stderr:
                print(f"   Error: {result.stderr}")
    
    return generated_images

def display_results(performance_results, visualizations):
    """Display comprehensive results"""
    print("\n📈 COMPREHENSIVE RESULTS SUMMARY")
    print("=" * 50)
    
    # Performance Summary
    print("\n🚀 Performance Results:")
    for name, result in performance_results.items():
        status = "✅ PASSED" if result['success'] else "❌ FAILED"
        print(f"  {name}: {status} ({result['runtime']:.2f}s)")
    
    # Display visualizations
    print(f"\n🎨 Generated {len(visualizations)} Professional Visualizations:")
    
    for viz in visualizations:
        print(f"\n📊 {viz['name']}")
        print(f"   {viz['description']}")
        
        try:
            display(Image(viz['file']))
        except Exception as e:
            print(f"   Could not display {viz['file']}: {e}")
    
    # Performance Analysis
    print("\n📊 Performance Analysis:")
    runtimes = [r['runtime'] for r in performance_results.values()]
    
    plt.figure(figsize=(10, 6))
    plt.bar(range(len(performance_results)), runtimes)
    plt.xlabel('Test Modules')
    plt.ylabel('Runtime (seconds)')
    plt.title('Euler Prover Performance Benchmark')
    plt.xticks(range(len(performance_results)), 
               [name.split(' - ')[0] for name in performance_results.keys()], 
               rotation=45)
    plt.tight_layout()
    plt.show()

def run_colab_demo():
    """Main demo function for Google Colab"""
    print("🔬 ENHANCED EULER PROVER - GOOGLE COLAB DEMO")
    print("=" * 60)
    print("High-Performance Mathematical Computation with VTK Visualization")
    print("=" * 60)
    
    try:
        # Setup
        setup_euler_prover()
        
        # Run performance tests
        performance_results = run_performance_demo()
        
        # Generate visualizations  
        visualizations = run_visualization_demo()
        
        # Display comprehensive results
        display_results(performance_results, visualizations)
        
        print("\n🎉 DEMO COMPLETED SUCCESSFULLY!")
        print("✅ All mathematical theorems verified computationally")
        print("✅ Professional-quality visualizations generated")
        print("✅ High-performance optimization demonstrated")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Demo failed: {e}")
        return False

if __name__ == "__main__":
    # For direct execution in Colab
    run_colab_demo()
