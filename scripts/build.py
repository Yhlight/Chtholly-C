import os
import subprocess
import argparse
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Executing: {' '.join(command)}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Build script for the Chtholly project.")
    parser.add_argument("--test", action="store_true", help="Run tests after building.")
    args = parser.parse_args()

    # Get the project root directory (the parent of the 'scripts' directory)
    project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    build_dir = os.path.join(project_root, "build")

    # Create the build directory if it doesn't exist
    os.makedirs(build_dir, exist_ok=True)

    # Configure the project with CMake
    cmake_configure_command = ["cmake", ".."]
    run_command(cmake_configure_command, cwd=build_dir)

    # Build the project
    cmake_build_command = ["cmake", "--build", "."]
    run_command(cmake_build_command, cwd=build_dir)

    print("Build successful.")

    if args.test:
        print("Running tests...")
        # The test executable will be in 'build/tests/'
        test_executable = os.path.join(build_dir, "tests", "ChthollyTests")
        run_command([test_executable], cwd=os.path.join(build_dir, "tests"))

if __name__ == "__main__":
    main()
