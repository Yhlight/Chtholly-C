import os
import sys
import subprocess

def run_command(args, cwd=None):
    """Runs a command and prints its output."""
    print(f"Executing: {' '.join(args)}")
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=cwd)
    for line in iter(process.stdout.readline, b''):
        print(line.decode('utf-8').strip())
    process.stdout.close()
    return process.wait()

def main():
    if len(sys.argv) < 2:
        print("Usage: python build.py [build|test|clean]")
        return

    command = sys.argv[1]
    build_dir = "build"

    if command == "build":
        os.makedirs(build_dir, exist_ok=True)
        cmake_args = ["cmake", "-B", build_dir, "-S", "."]
        if run_command(cmake_args) != 0:
            print("CMake configuration failed.")
            return

        build_args = ["cmake", "--build", build_dir]
        if run_command(build_args) != 0:
            print("Build failed.")
            return

    elif command == "test":
        test_args = ["ctest", "--test-dir", build_dir]
        if run_command(test_args) != 0:
            print("Tests failed.")
            return

    elif command == "clean":
        if os.path.exists(build_dir):
            run_command(["rm", "-rf", build_dir])
            print(f"Cleaned the build directory: {build_dir}")

    else:
        print(f"Unknown command: {command}")

if __name__ == "__main__":
    main()
