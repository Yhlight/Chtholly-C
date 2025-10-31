#!/usr/bin/env python3
import argparse
import subprocess
import sys
import os

def run_command(command):
    """Executes a command and exits if it fails."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Command '{e.cmd}' failed with exit code {e.returncode}", file=sys.stderr)
        sys.exit(e.returncode)

def build():
    """Configures and builds the project."""
    os.makedirs("build", exist_ok=True)
    run_command("cmake -B build -S .")
    run_command("cmake --build build")

def test():
    """Runs the tests."""
    test_executable = os.path.join("build", "tests", "chtholly_tests")
    if not os.path.exists(test_executable):
        print(f"Test executable not found at: {test_executable}", file=sys.stderr)
        print("Please build the project first using 'python build.py build'", file=sys.stderr)
        sys.exit(1)
    run_command(test_executable)

def main():
    parser = argparse.ArgumentParser(description="Chtholly build script")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("build", help="Build the project")
    subparsers.add_parser("test", help="Run tests")

    args = parser.parse_args()

    if args.command == "build":
        build()
    elif args.command == "test":
        # Ensure the project is built before testing
        build()
        test()

if __name__ == "__main__":
    main()
