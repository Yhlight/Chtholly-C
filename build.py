import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {command}")
        sys.exit(1)

def main():
    """Builds and runs the Chtholly compiler."""
    # Create build directory
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure CMake
    run_command("cmake -B build -S .")

    # Build the project
    run_command("cmake --build build")

    # Run the executable
    if sys.platform == "win32":
        run_command(".\\build\\chtholly.exe")
    else:
        run_command("./build/chtholly")

if __name__ == "__main__":
    main()
