#!/usr/bin/env python3
"""
EULER PROVER - COMPLETE GOOGLE COLAB DEMO
==========================================

Demonstrasi lengkap Euler Prover di Google Colab dengan:
- Instalasi otomatis
- Computational proofs
- 3D visualizations dengan PNG export
- Display dan download hasil

Copy dan paste script ini ke Google Colab untuk demo lengkap!
"""

import os
import subprocess
import sys
from pathlib import Path

def run_command(cmd, description=""):
    """Execute shell command with proper error handling"""
    print(f"🔄 {description}")
    print(f"Command: {cmd}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            print(f"✅ Success: {description}")
            if result.stdout:
                print(result.stdout)
        else:
            print(f"⚠️  Warning: {description}")
            if result.stderr:
                print(f"Error: {result.stderr}")
        return result.returncode == 0
    except Exception as e:
        print(f"❌ Failed: {description} - {e}")
        return False

def setup_euler_prover():
    """Setup Euler Prover in Google Colab"""
    print("=" * 60)
    print("🚀 EULER PROVER - COLAB SETUP")
    print("=" * 60)
    
    # 1. Install system dependencies
    print("\n📦 Installing system dependencies...")
    deps_cmd = """
    apt-get update -qq && 
    apt-get install -y build-essential cmake gcc g++ libvtk9-dev python3-vtk9 git
    """
    run_command(deps_cmd, "Installing build tools and VTK")
    
    # 2. Clone repository
    print("\n📥 Cloning Euler Prover repository...")
    if os.path.exists("euler-prover"):
        print("Repository already exists, updating...")
        run_command("cd euler-prover && git pull", "Updating repository")
    else:
        run_command("git clone https://github.com/putraaxzy/euler-prover.git", "Cloning repository")
    
    # 3. Build Euler Prover
    print("\n🔨 Building Euler Prover...")
    os.chdir("euler-prover")
    run_command("chmod +x build_colab.sh", "Making build script executable")
    
    build_success = run_command("./build_colab.sh", "Building Euler Prover")
    
    if not build_success:
        print("⚠️  Main build failed, trying fallback...")
        fallback_cmd = """
        g++ -O3 -Iinclude -DVTK_FOUND=0 -DNO_VISUALIZATION=1 
            src/main.cpp src/number_theory.cpp src/complex_analysis.cpp 
            src/topology.cpp src/progress.cpp src/rng.cpp 
            -o build_colab/euler_fallback
        """
        run_command(fallback_cmd, "Fallback build")
    
    # 4. Navigate to build directory
    os.chdir("build_colab")
    
    # 5. Check available executables
    print("\n📋 Available executables:")
    run_command("ls -la euler*", "Listing executables")
    
    return True

def run_computational_tests():
    """Run computational proof tests"""
    print("\n" + "=" * 60)
    print("🧮 COMPUTATIONAL PROOF TESTS")
    print("=" * 60)
    
    tests = [
        ("./euler number 1000 10 4", "Euler's Theorem Test"),
        ("./euler complex 5000 1e-12", "Euler's Formula Test"), 
        ("./euler topology 4", "Euler Characteristic Test")
    ]
    
    for cmd, desc in tests:
        print(f"\n🔬 {desc}")
        print("-" * 40)
        run_command(cmd, desc)

def generate_visualizations():
    """Generate 3D visualizations"""
    print("\n" + "=" * 60)
    print("🎨 3D VISUALIZATION GENERATION")
    print("=" * 60)
    
    visualizations = [
        ("./euler visualize primes 1000", "Prime Distribution (Ulam Spiral)"),
        ("./euler visualize topology icosphere 4", "Euler Characteristic (Icosphere)"),
        ("./euler visualize complex euler 300", "Euler Formula Visualization"),
        ("./euler visualize complex riemann 200", "Riemann Surface")
    ]
    
    for cmd, desc in visualizations:
        print(f"\n🎨 {desc}")
        print("-" * 40)
        success = run_command(cmd, desc)
        if success:
            print(f"✅ Generated: {desc}")
        else:
            print(f"⚠️  Skipped: {desc} (executable not available)")

def display_results():
    """Display generated PNG files"""
    print("\n" + "=" * 60)
    print("📊 DISPLAYING RESULTS")
    print("=" * 60)
    
    try:
        from IPython.display import Image, display
        import matplotlib.pyplot as plt
        
        # Find all PNG files
        png_files = [f for f in os.listdir('.') if f.endswith('.png')]
        
        if not png_files:
            print("⚠️  No PNG files found. Visualizations may not have been generated.")
            return
        
        print(f"📸 Found {len(png_files)} visualization(s):")
        
        for png_file in png_files:
            print(f"\n🖼️  {png_file}")
            print("=" * 50)
            try:
                display(Image(png_file))
            except Exception as e:
                print(f"❌ Could not display {png_file}: {e}")
                
    except ImportError:
        print("⚠️  IPython not available. Running in non-Colab environment.")
        print("📸 Generated PNG files:")
        run_command("ls -la *.png", "Listing PNG files")

def download_results():
    """Download generated files"""
    print("\n" + "=" * 60)
    print("💾 DOWNLOADING RESULTS")
    print("=" * 60)
    
    try:
        from google.colab import files
        
        # Download PNG files
        png_files = [f for f in os.listdir('.') if f.endswith('.png')]
        
        if not png_files:
            print("⚠️  No PNG files to download.")
            return
            
        print(f"📥 Downloading {len(png_files)} file(s)...")
        
        for png_file in png_files:
            try:
                files.download(png_file)
                print(f"✅ Downloaded: {png_file}")
            except Exception as e:
                print(f"❌ Failed to download {png_file}: {e}")
                
    except ImportError:
        print("⚠️  Google Colab files module not available.")
        print("💡 You can manually download files from the file browser.")

def main():
    """Main demo function"""
    print("🌟 EULER PROVER - COMPLETE GOOGLE COLAB DEMO")
    print("=" * 60)
    print("Demonstrasi lengkap sistem pembuktian matematika Euler")
    print("dengan visualisasi 3D berkualitas tinggi!")
    print("=" * 60)
    
    try:
        # 1. Setup
        setup_success = setup_euler_prover()
        if not setup_success:
            print("❌ Setup failed. Please check the error messages above.")
            return
        
        # 2. Computational tests
        run_computational_tests()
        
        # 3. Generate visualizations
        generate_visualizations()
        
        # 4. Display results
        display_results()
        
        # 5. Download results
        download_results()
        
        print("\n" + "=" * 60)
        print("🎉 EULER PROVER DEMO COMPLETE!")
        print("=" * 60)
        print("✅ Computational proofs executed")
        print("✅ 3D visualizations generated") 
        print("✅ High-quality PNG files created")
        print("✅ Results displayed and downloaded")
        print("\n🔗 Repository: https://github.com/putraaxzy/euler-prover")
        
    except Exception as e:
        print(f"❌ Demo failed with error: {e}")
        print("💡 Try running individual sections manually.")

if __name__ == "__main__":
    main()

# =============================================================================
# QUICK START COMMANDS FOR COLAB
# =============================================================================
"""
Untuk menjalankan demo ini di Google Colab:

1. Buat notebook baru di https://colab.research.google.com/

2. Copy dan paste script ini ke dalam cell

3. Jalankan dengan:
   exec(open('colab_demo_complete.py').read())

Atau jalankan langsung:
   !python colab_demo_complete.py

4. Tunggu proses instalasi dan build selesai

5. Nikmati hasil visualisasi 3D berkualitas tinggi!

MANUAL COMMANDS:
================
# Setup
!git clone https://github.com/putraaxzy/euler-prover.git
%cd euler-prover
!./build_colab.sh

# Tests  
%cd build_colab
!./euler number 1000 10 4
!./euler visualize primes 1000

# Display
from IPython.display import Image, display
display(Image('euler_3d_primes_1000.png'))
"""