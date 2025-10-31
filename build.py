import os
import subprocess
import argparse
import sys

def main():
    parser = argparse.ArgumentParser(description="Chtholly build script")
    parser.add_argument("--run", action="store_true", help="Run the compiled executable")
    args = parser.parse_args()

    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    try:
        # Configure the project with CMake
        configure_command = ["cmake", "-S", ".", "-B", build_dir]
        print(f"Running: {' '.join(configure_command)}")
        subprocess.run(configure_command, check=True, cwd=os.getcwd())

        # Build the project with CMake
        build_command = ["cmake", "--build", build_dir]
        print(f"Running: {' '.join(build_command)}")
        subprocess.run(build_command, check=True, cwd=os.getcwd())

    except subprocess.CalledProcessError as e:
        print(f"An error occurred during the build process: {e}", file=sys.stderr)
        sys.exit(1)
    except FileNotFoundError:
        print("Error: `cmake` command not found. Please ensure CMake is installed and in your PATH.", file=sys.stderr)
        sys.exit(1)


    if args.run:
        executable_path = os.path.join(build_dir, "chtholly")
        if os.path.exists(executable_path):
            print(f"Running executable: {executable_path}")
            try:
                subprocess.run([executable_path], check=True, cwd=os.getcwd())
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while running the executable: {e}", file=sys.stderr)
                sys.exit(1)
        else:
            print(f"Error: Executable not found at {executable_path}", file=sys.stderr)
            sys.exit(1)

if __name__ == "__main__":
    main()
