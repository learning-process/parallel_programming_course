#!/usr/bin/env python3
"""
Generate LLVM coverage reports from profraw files.

This script merges LLVM profile data files and generates coverage reports
in various formats (LCOV, HTML) using llvm-profdata and llvm-cov tools.
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path


def find_profile_files(build_dir):
    """Find all .profraw files in the build directory."""
    profile_files = list(Path(build_dir).rglob("*.profraw"))
    if not profile_files:
        print("No profile files found!", file=sys.stderr)
        print(f"Searched in: {Path(build_dir).absolute()}", file=sys.stderr)
        # List all files to debug
        print("Files in build directory:", file=sys.stderr)
        for f in Path(build_dir).iterdir():
            if f.is_file():
                print(f"  {f.name}", file=sys.stderr)
        sys.exit(1)
    print(f"Found {len(profile_files)} profile files:")
    for f in profile_files:
        print(f"  {f}")
    return profile_files


def merge_profiles(profile_files, output_file, llvm_profdata="llvm-profdata"):
    """Merge multiple profile files into a single profdata file."""
    cmd = ([llvm_profdata, "merge", "-sparse"] +
           [str(f) for f in profile_files] + ["-o", output_file])
    print(f"Merging {len(profile_files)} profile files...")
    subprocess.run(cmd, check=True)
    print(f"Merged profile data written to: {output_file}")


def generate_coverage_reports(
        profdata_file,
        build_dir,
        output_dir,
        llvm_cov="llvm-cov",
        executables=None
):
    """Generate coverage reports using llvm-cov."""
    if executables is None:
        executables = ["bin/ppc_func_tests", "bin/core_func_tests"]

    ignore_patterns = None
    if ignore_patterns is None:
        ignore_patterns = [
            '.*3rdparty/.*',
            '/usr/.*',
            '.*tasks/.*/tests/.*',
            '.*modules/.*/tests/.*',
            '.*tasks/common/runners/.*',
            '.*modules/runners/.*',
            '.*modules/util/include/perf_test_util.hpp',
            '.*modules/util/include/func_test_util.hpp',
            '.*modules/util/src/func_test_util.cpp'
        ]

    # Build the executable list
    exec_paths = []
    print(f"\nLooking for executables in {Path(build_dir).absolute()}:")
    for exe in executables:
        exe_path = Path(build_dir) / exe
        if exe_path.exists():
            exec_paths.append(str(exe_path))
            print(f"  Found: {exe_path}")
        else:
            print(f"  Warning: Executable not found: {exe_path}", file=sys.stderr)
            # Try to find similar executables
            bin_dir = Path(build_dir) / "bin"
            if bin_dir.exists():
                print(f"  Available executables in bin/:", file=sys.stderr)
                for f in bin_dir.iterdir():
                    if f.is_file() and f.stat().st_mode & 0o111:  # executable
                        print(f"    {f.name}", file=sys.stderr)

    if not exec_paths:
        print("No executables found!", file=sys.stderr)
        sys.exit(1)

    # Build ignore regex arguments
    ignore_args = []
    for pattern in ignore_patterns:
        ignore_args.extend(["-ignore-filename-regex", pattern])

    # Generate coverage summary (to console)
    print("\nGenerating coverage summary...")
    cmd = [llvm_cov, "report"]
    if exec_paths:
        cmd.append(exec_paths[0])  # First executable
        for exe in exec_paths[1:]:
            cmd.extend(["-object", exe])  # Additional executables
    cmd.extend(["-instr-profile", profdata_file] + ignore_args)
    subprocess.run(cmd, check=True)

    # Generate LCOV report
    # Place coverage.lcov in the current working directory (build dir)
    lcov_file = Path("coverage.lcov")
    print(f"\nGenerating LCOV report: {lcov_file}")
    
    # For llvm-cov export, we need to specify the object files differently
    # The first executable is the main object, others are specified with -object
    cmd = [llvm_cov, "export"]
    if exec_paths:
        cmd.append(exec_paths[0])  # First executable
        for exe in exec_paths[1:]:
            cmd.extend(["-object", exe])  # Additional executables
    cmd.extend([
        "-instr-profile", profdata_file,
        "-format=lcov"
    ] + ignore_args)
    
    print(f"Running: {' '.join(cmd[:10])}...")  # Print first part of command for debugging

    with open(lcov_file, 'w', encoding='utf-8') as f:
        result = subprocess.run(cmd, stdout=f, stderr=subprocess.PIPE, text=True)
        if result.returncode != 0:
            print(f"Error generating LCOV: {result.stderr}", file=sys.stderr)
            raise subprocess.CalledProcessError(result.returncode, cmd)

    # Generate HTML report
    html_dir = Path(output_dir)
    html_dir.mkdir(parents=True, exist_ok=True)
    print(f"\nGenerating HTML report: {html_dir}")
    cmd = [llvm_cov, "show"]
    if exec_paths:
        cmd.append(exec_paths[0])  # First executable
        for exe in exec_paths[1:]:
            cmd.extend(["-object", exe])  # Additional executables
    cmd.extend([
        "-instr-profile", profdata_file,
        "-format=html",
        "-output-dir", str(html_dir),
        "-show-line-counts-or-regions",
        "-show-instantiations"
    ] + ignore_args)

    subprocess.run(cmd, check=True)
    print("\nCoverage reports generated successfully!")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--build-dir",
        default="build",
        help="Build directory containing profile files and executables (default: build)"
    )
    parser.add_argument(
        "--output-dir",
        default="cov-report",
        help="Output directory for HTML coverage report (default: cov-report)"
    )
    parser.add_argument(
        "--llvm-profdata",
        default="llvm-profdata",
        help="Path to llvm-profdata tool (default: llvm-profdata)"
    )
    parser.add_argument(
        "--llvm-cov",
        default="llvm-cov",
        help="Path to llvm-cov tool (default: llvm-cov)"
    )
    parser.add_argument(
        "--profdata-file",
        help="Use existing merged profdata file instead of merging .profraw files"
    )
    parser.add_argument(
        "--executables",
        nargs="+",
        help="List of executables to analyze (relative to build-dir)"
    )

    args = parser.parse_args()

    # Change to build directory
    os.chdir(args.build_dir)

    # Merge profiles if needed
    if args.profdata_file:
        profdata_file = args.profdata_file
    else:
        profile_files = find_profile_files(".")
        profdata_file = "merged.profdata"
        merge_profiles(profile_files, profdata_file, args.llvm_profdata)

    # Generate reports
    generate_coverage_reports(
        profdata_file,
        ".",
        args.output_dir,
        args.llvm_cov,
        args.executables
    )


if __name__ == "__main__":
    main()
