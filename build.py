import os
import subprocess
import argparse

def run_command(command):
    """Runs a command and checks for errors."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e}")
        exit(1)

def main():
    parser = argparse.ArgumentParser(description="Build and run the Chtholly compiler.")
    parser.add_argument("--run", action="store_true", help="Run the compiler after building.")
    parser.add_argument("--source", type=str, help="Source file to compile.")
    parser.add_argument("--output", type=str, help="Output file for the compiled code.")
    args = parser.parse_args()

    # Create a build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure the project with CMake
    run_command("cmake -B build")

    # Build the project
    run_command("cmake --build build")

    if args.run:
        if not args.source or not args.output:
            print("Error: --source and --output are required when using --run.")
            exit(1)

        compiler_path = os.path.join("build", "chtholly")
        run_command(f"{compiler_path} {args.source} {args.output}")

if __name__ == "__main__":
    main()
