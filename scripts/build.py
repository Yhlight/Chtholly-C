import os
import subprocess
import sys

def main():
    """
    Builds the Chtholly project using CMake and Make.
    """
    build_dir = "build"

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project with CMake
    cmake_command = ["cmake", ".."]
    print(f"Running command in '{build_dir}': {' '.join(cmake_command)}")
    result = subprocess.run(cmake_command, cwd=build_dir)
    if result.returncode != 0:
        print("CMake configuration failed.")
        sys.exit(1)

    # Build the project with make
    make_command = ["make"]
    print(f"Running command in '{build_dir}': {' '.join(make_command)}")
    result = subprocess.run(make_command, cwd=build_dir)
    if result.returncode != 0:
        print("Build failed.")
        sys.exit(1)

    print("\\nBuild successful!")
    print(f"Executables are in the '{build_dir}' directory.")


if __name__ == "__main__":
    main()
