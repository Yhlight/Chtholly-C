import os
import subprocess
import sys
import argparse

def run_command(args, cwd=None):
    """Runs a command and prints its output in real-time."""
    print(f"Running command: {' '.join(args)}")
    try:
        process = subprocess.Popen(
            args,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            encoding='utf-8',
            errors='replace',
            cwd=cwd
        )
        while True:
            output = process.stdout.readline()
            if output == '' and process.poll() is not None:
                break
            if output:
                print(output.strip(), flush=True)
        return process.poll()
    except FileNotFoundError:
        print(f"Error: Command '{args[0]}' not found. Make sure it's in your PATH.")
        return -1
    except Exception as e:
        print(f"An error occurred: {e}")
        return -1

def find_executable(build_dir, project_name):
    """Finds the executable in the build directory."""
    for root, _, files in os.walk(build_dir):
        for file in files:
            if file == project_name or file == f"{project_name}.exe":
                return os.path.join(root, file)
    return None

def main():
    """Main function to drive the build process."""
    parser = argparse.ArgumentParser(description="Chtholly compiler build script.")
    parser.add_argument('action', nargs='?', default='build', choices=['configure', 'build', 'run', 'rebuild'], help="Action to perform.")
    parser.add_argument('--project-name', default='Chtholly', help="The name of the CMake project.")
    parser.add_argument('--build-dir', default='build', help="The directory for build artifacts.")

    args = parser.parse_args()

    action = args.action
    project_name = args.project_name
    build_dir = args.build_dir

    if action == 'rebuild':
        if os.path.exists(build_dir):
            import shutil
            print(f"Removing build directory: {build_dir}")
            shutil.rmtree(build_dir)
        action = 'build' # Continue to a full build

    # Configure
    if action in ['configure', 'build', 'run']:
        if not os.path.exists(os.path.join(build_dir, 'CMakeCache.txt')):
            print("--- Configuring CMake ---")
            os.makedirs(build_dir, exist_ok=True)
            cmake_args = ["cmake", "-S", ".", "-B", build_dir]
            if "CMAKE_CXX_COMPILER" in os.environ:
                cmake_args.append(f"-DCMAKE_CXX_COMPILER={os.environ['CMAKE_CXX_COMPILER']}")
            if "CMAKE_C_COMPILER" in os.environ:
                cmake_args.append(f"-DCMAKE_C_COMPILER={os.environ['CMAKE_C_COMPILER']}")
            if "CMAKE_CXX_FLAGS" in os.environ:
                cmake_args.append(f"-DCMAKE_CXX_FLAGS={os.environ['CMAKE_CXX_FLAGS']}")

            if run_command(cmake_args) != 0:
                print("CMake configuration failed.")
                sys.exit(1)
        if action == 'configure':
            return

    # Build
    if action in ['build', 'run']:
        print("\n--- Building Project ---")
        cmake_build_args = ["cmake", "--build", build_dir]
        if run_command(cmake_build_args) != 0:
            print("Build failed.")
            sys.exit(1)
        if action == 'build':
            return

    # Run
    if action == 'run':
        print("\n--- Running Executable ---")
        executable_path = find_executable(build_dir, project_name)
        if executable_path and os.path.exists(executable_path):
            run_command([executable_path])
        else:
            print(f"Error: Executable '{project_name}' not found in '{build_dir}'.")
            sys.exit(1)

if __name__ == "__main__":
    main()
