import os
import subprocess
import argparse
import sys

def run_command(command, cwd=None):
    """Runs a command and checks for errors."""
    print(f"Running command: {' '.join(command)}")
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        print(f"stdout:\\n{result.stdout}")
        print(f"stderr:\\n{result.stderr}")
        sys.exit(1)
    return result

def main():
    parser = argparse.ArgumentParser(description="Build script for the Chtholly compiler.")
    parser.add_argument("--run", action="store_true", help="Run the full compile-and-run cycle.")
    args = parser.parse_args()

    # Create build directory
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # --- Step 1: Build the Chtholly compiler ---
    run_command(["cmake", ".."], cwd=build_dir)
    run_command(["cmake", "--build", "."], cwd=build_dir)

    if args.run:
        # --- Step 2: Run Chtholly compiler to generate C++ code ---
        chtholly_executable = os.path.join(build_dir, "chtholly")
        output_cpp_file = os.path.join(build_dir, "output.cpp")
        run_command([chtholly_executable, output_cpp_file])

        # --- Step 3: Compile the generated C++ code with g++ ---
        output_executable = os.path.join(build_dir, "output_executable")
        run_command(["g++", "-std=c++20", output_cpp_file, "-o", output_executable])

        # --- Step 4: Run the final executable ---
        print("\\n--- Running transpiled code ---")
        result = run_command([output_executable])
        print(result.stdout, end="")
        print("--- End of transpiled code output ---\\n")

if __name__ == "__main__":
    main()
