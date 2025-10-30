import os
import subprocess
import argparse
import sys

def run_command(command):
    """Runs a command and checks for errors."""
    print(f"Executing: {' '.join(command)}")
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        print(f"Stdout:\n{result.stdout}")
        print(f"Stderr:\n{result.stderr}")
        sys.exit(1)
    return result

def main():
    parser = argparse.ArgumentParser(description="Chtholly compiler build script.")
    parser.add_argument("--run-tests", action="store_true", help="Run tests after building.")
    args = parser.parse_args()

    # Create build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure the project
    cmake_configure_command = [
        "cmake",
        "-S",
        ".",
        "-B",
        "build",
        "-DCMAKE_BUILD_TYPE=Debug"
    ]
    run_command(cmake_configure_command)

    # Build the project
    cmake_build_command = [
        "cmake",
        "--build",
        "build"
    ]
    run_command(cmake_build_command)

    print("\nBuild successful!")

    if args.run_tests:
        print("\nRunning tests...")
        ctest_command = ["ctest", "--test-dir", "build", "--output-on-failure"]
        run_command(ctest_command)

if __name__ == "__main__":
    main()
