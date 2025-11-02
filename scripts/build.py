import os
import subprocess
import argparse

def run_command(command):
    """Runs a command and checks for errors."""
    print(f"Executing: {' '.join(command)}")
    result = subprocess.run(command, check=False)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        exit(1)

def main():
    """Main function to handle the build process."""
    parser = argparse.ArgumentParser(description="Chtholly build script")
    parser.add_argument("--run", action="store_true", help="Run the compiler after building")
    parser.add_argument("--test", action="store_true", help="Run tests after building")
    parser.add_argument("file", nargs="?", help="The file to run")
    args = parser.parse_args()

    # Create build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure CMake
    run_command(["cmake", "-S", ".", "-B", "build"])

    # Build the project
    run_command(["cmake", "--build", "build"])

    if args.run:
        executable = os.path.join("build", "src", "chtholly")
        if args.file:
            run_command([executable, args.file])
        else:
            run_command([executable])

    if args.test:
        run_command(["ctest", "--test-dir", "build", "--output-on-failure"])

if __name__ == "__main__":
    main()
