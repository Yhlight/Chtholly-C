import os
import subprocess
import argparse
import sys

def run_command(command, cwd=None):
    """Runs a command and checks for errors."""
    try:
        result = subprocess.run(command, check=True, cwd=cwd, text=True, capture_output=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(command)}", file=sys.stderr)
        print(e.stderr, file=sys.stderr)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Build the Chtholly project using CMake.")
    parser.add_argument("--run", action="store_true", help="Run the compiled executable after building.")
    args = parser.parse_args()

    build_dir = "build"
    project_name = "chtholly" # As per convention, can be changed later

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project with CMake
    print("--- Configuring project with CMake ---")
    run_command(["cmake", ".."], cwd=build_dir)

    # Build the project
    print("\n--- Building project ---")
    run_command(["cmake", "--build", "."], cwd=build_dir)

    # Run the executable if requested
    if args.run:
        print(f"\n--- Running {project_name} ---")
        executable_path = os.path.join(build_dir, project_name)
        if os.path.exists(executable_path):
            run_command([f"./{project_name}"], cwd=build_dir)
        else:
            print(f"Error: Executable not found at {executable_path}", file=sys.stderr)
            sys.exit(1)

if __name__ == "__main__":
    main()
