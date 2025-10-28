import os
import subprocess
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)}")
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running command: {' '.join(command)}")
        print(f"Stdout: {result.stdout}")
        print(f"Stderr: {result.stderr}")
        sys.exit(1)
    print(result.stdout)
    return result

def main():
    """Main function to build and run the Chtholly compiler."""
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    build_dir = os.path.join(repo_root, "build")

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project with CMake
    cmake_configure_command = ["cmake", "-DCMAKE_BUILD_TYPE=Debug", ".."]
    run_command(cmake_configure_command, cwd=build_dir)

    # Build the project
    cmake_build_command = ["cmake", "--build", "."]
    run_command(cmake_build_command, cwd=build_dir)

    # Run the tests
    ctest_command = ["ctest", "--output-on-failure"]
    print("\\n--- Running Tests ---")
    run_command(ctest_command, cwd=build_dir)
    print("--- Tests finished ---\\n")

    # Run the executable
    executable_path = os.path.join(build_dir, "chtholly")
    test_file_path = os.path.join(repo_root, "test.cns")
    if os.path.exists(executable_path):
        print("\\n--- Running Chtholly ---")
        run_command([executable_path, test_file_path], cwd=repo_root)
        print("--- Chtholly finished ---\\n")
    else:
        print(f"Error: Executable not found at {executable_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()
