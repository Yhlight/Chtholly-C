import os
import subprocess
import sys
import argparse

def run_command(command):
    """Runs a command and exits if it fails."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e}")
        sys.exit(1)

def main():
    """Configures, builds, and optionally runs the project."""
    parser = argparse.ArgumentParser(description="Build and run the Chtholly project.")
    parser.add_argument("--run", nargs='?', const="", default=None,
                        help="Run the Chtholly compiler. Optionally provide a file path.")
    parser.add_argument("--test", action="store_true",
                        help="Run the test suite.")
    args = parser.parse_args()

    build_dir = "build"
    chtholly_executable = os.path.join(build_dir, "src", "chtholly")
    tests_executable = os.path.join(build_dir, "tests", "chtholly_tests")

    # Configure and build the project
    if not os.path.exists(chtholly_executable) or not os.path.exists(tests_executable):
        print("Build artifacts not found. Running a full build...")
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        run_command(f"cmake -S . -B {build_dir}")
        run_command(f"cmake --build {build_dir}")
    else:
        print("Build artifacts found. Skipping build. To force a rebuild, delete the 'build' directory.")


    # Run the appropriate executable if requested
    if args.run is not None:
        command = f"{chtholly_executable}"
        if args.run: # If a file path was provided
            command += f" {args.run}"
        print(f"Running: {command}")
        run_command(command)

    if args.test:
        print(f"Running tests: {tests_executable}")
        run_command(tests_executable)

if __name__ == "__main__":
    main()
