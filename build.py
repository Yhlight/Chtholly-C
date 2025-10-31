import os
import subprocess
import sys
import argparse

def run_command(command):
    """Runs a command and prints its output."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {e}")
        sys.exit(1)

def main():
    """Builds the Chtholly project."""
    parser = argparse.ArgumentParser(description="Build and run the Chtholly project.")
    parser.add_argument("--run", action="store_true", help="Run the compiled executable after building.")
    args = parser.parse_args()

    if not os.path.exists("build"):
        os.makedirs("build")

    os.chdir("build")

    run_command("cmake ..")
    run_command("cmake --build .")

    os.chdir("..")

    print("Build successful!")

    if args.run:
        print("\nRunning chtholly:")
        run_command("./build/chtholly")

if __name__ == "__main__":
    main()
