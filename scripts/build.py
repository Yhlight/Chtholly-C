import os
import subprocess
import argparse

def run_command(command, cwd=None):
    """Runs a command and prints its output."""
    try:
        subprocess.run(command, check=True, shell=True, cwd=cwd)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {command}")
        print(e)
        exit(1)

def build(clean=False):
    """Builds the project."""
    if clean:
        run_command("rm -rf build")
    if not os.path.exists("build"):
        os.makedirs("build")
    run_command("cmake -S . -B build", cwd=os.getcwd())
    run_command("cmake --build build", cwd=os.getcwd())

def test():
    """Runs the tests."""
    run_command("ctest --test-dir build", cwd=os.getcwd())

def main():
    parser = argparse.ArgumentParser(description="Chtholly build script.")
    parser.add_argument("action", choices=["build", "test"], help="Action to perform.")
    parser.add_argument("--clean", action="store_true", help="Clean the build directory before building.")
    args = parser.parse_args()

    if args.action == "build":
        build(args.clean)
    elif args.action == "test":
        build()
        test()

if __name__ == "__main__":
    main()
