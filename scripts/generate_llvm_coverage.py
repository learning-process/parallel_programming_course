#!/usr/bin/env python3
"""Generate LLVM coverage report for the project."""

import os
import subprocess
import sys
import glob
import argparse


def run_command(cmd, cwd=None):
    """Run a command and return its output."""
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        sys.exit(1)
    return result.stdout


def main():
    parser = argparse.ArgumentParser(description="Generate LLVM coverage report")
    parser.add_argument("--build-dir", default="build", help="Build directory")
    parser.add_argument("--output-dir", default="coverage", help="Output directory for coverage report")
    parser.add_argument("--llvm-version", default="20", help="LLVM version (default: 20)")
    args = parser.parse_args()

    build_dir = os.path.abspath(args.build_dir)
    output_dir = os.path.abspath(args.output_dir)
    
    # Try to find LLVM tools in various locations
    llvm_profdata = None
    llvm_cov = None
    
    # List of possible LLVM tool names
    if args.llvm_version:
        profdata_names = [f"llvm-profdata-{args.llvm_version}", "llvm-profdata"]
        cov_names = [f"llvm-cov-{args.llvm_version}", "llvm-cov"]
    else:
        profdata_names = ["llvm-profdata"]
        cov_names = ["llvm-cov"]
    
    # Try to find the tools
    for name in profdata_names:
        result = subprocess.run(["which", name], capture_output=True, text=True)
        if result.returncode == 0:
            llvm_profdata = name
            break
    
    for name in cov_names:
        result = subprocess.run(["which", name], capture_output=True, text=True)
        if result.returncode == 0:
            llvm_cov = name
            break
    
    if not llvm_profdata or not llvm_cov:
        print("Error: Could not find llvm-profdata or llvm-cov in PATH")
        print("Make sure LLVM tools are installed and in your PATH")
        sys.exit(1)

    if not os.path.exists(build_dir):
        print(f"Error: Build directory {build_dir} does not exist")
        sys.exit(1)

    # Create output directory
    os.makedirs(output_dir, exist_ok=True)

    # Find all .profraw files
    profraw_files = glob.glob(os.path.join(build_dir, "**", "*.profraw"), recursive=True)
    if not profraw_files:
        print("No .profraw files found. Make sure to run tests with LLVM_PROFILE_FILE set.")
        print("Example: LLVM_PROFILE_FILE='coverage-%p-%m.profraw' ./your_test")
        sys.exit(1)

    print(f"Found {len(profraw_files)} .profraw files")

    # Merge profiles
    profdata_file = os.path.join(output_dir, "coverage.profdata")
    run_command([llvm_profdata, "merge", "-sparse"] + profraw_files + ["-o", profdata_file])
    print(f"Created merged profile: {profdata_file}")

    # Find all executables in bin directory
    bin_dir = os.path.join(build_dir, "bin")
    if not os.path.exists(bin_dir):
        print(f"Error: Bin directory {bin_dir} does not exist")
        sys.exit(1)

    executables = []
    for root, dirs, files in os.walk(bin_dir):
        for file in files:
            filepath = os.path.join(root, file)
            if os.access(filepath, os.X_OK) and not file.endswith('.txt'):
                executables.append(filepath)

    if not executables:
        print("No executables found in bin directory")
        sys.exit(1)

    print(f"Found {len(executables)} executables")

    # Get the project root directory (parent of build dir)
    project_root = os.path.dirname(build_dir)
    
    # Generate LCOV report
    lcov_file = os.path.join(output_dir, "coverage.lcov")
    cmd = [llvm_cov, "export"] + executables + [
        "--format=lcov",
        "--ignore-filename-regex=.*3rdparty/.*|/usr/.*|.*tests/.*|"
        ".*tasks/.*|.*modules/runners/.*|.*modules/util/include/perf_test_util.hpp|"
        ".*modules/util/include/func_test_util.hpp|.*modules/util/src/func_test_util.cpp",
        f"--instr-profile={profdata_file}"
    ]
    
    with open(lcov_file, "w") as f:
        result = subprocess.run(cmd, stdout=f, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error generating LCOV report: {result.stderr}")
            sys.exit(1)
    
    print(f"Generated LCOV report: {lcov_file}")
    
    # Post-process LCOV file to use relative paths
    with open(lcov_file, 'r') as f:
        lcov_content = f.read()
    
    # Replace absolute paths with relative paths
    lcov_content = lcov_content.replace(project_root + '/', '')
    
    with open(lcov_file, 'w') as f:
        f.write(lcov_content)
    
    print("Post-processed LCOV file to use relative paths")

    # Generate HTML report
    html_dir = os.path.join(output_dir, "html")
    cmd = [llvm_cov, "show"] + executables + [
        "--format=html",
        f"--output-dir={html_dir}",
        "--ignore-filename-regex=.*3rdparty/.*|/usr/.*|.*tests/.*|"
        ".*tasks/.*|.*modules/runners/.*|.*modules/util/include/perf_test_util.hpp|"
        ".*modules/util/include/func_test_util.hpp|.*modules/util/src/func_test_util.cpp",
        f"--instr-profile={profdata_file}"
    ]
    
    run_command(cmd)
    print(f"Generated HTML report: {html_dir}/index.html")

    # Generate summary
    cmd = [llvm_cov, "report"] + executables + [
        f"--instr-profile={profdata_file}",
        "--ignore-filename-regex=.*3rdparty/.*|/usr/.*|.*tasks/.*/tests/.*|.*modules/.*/tests/.*|"
        ".*tasks/common/runners/.*|.*modules/runners/.*|.*modules/util/include/perf_test_util.hpp|"
        ".*modules/util/include/func_test_util.hpp|.*modules/util/src/func_test_util.cpp"
    ]
    
    summary = run_command(cmd)
    print("\nCoverage Summary:")
    print(summary)


if __name__ == "__main__":
    main()