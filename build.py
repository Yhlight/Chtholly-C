import os
import subprocess
import argparse
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="CMake build script for Chtholly.")
    parser.add_argument("--run", action="store_true", help="Run the executable after building.")
    args = parser.parse_args()

    # Create build directory
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure CMake
    run_command(["cmake", ".."], cwd=build_dir)

    # Build the project
    run_command(["cmake", "--build", "."], cwd=build_dir)

    # Run the executable if requested
    if args.run:
        executable_path = os.path.join(build_dir, "chtholly")
        run_command([executable_path], cwd=".")

if __name__ == "__main__":
    main()
