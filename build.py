#!/usr/bin/env python3
import os
import subprocess
import sys

def run_command(args, env=None):
    """Runs a command and exits if it fails."""
    print(f"Running command: {' '.join(args)}")
    result = subprocess.run(args, env=env)
    if result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def main():
    if len(sys.argv) < 2:
        print("Usage: build.py <build|test|run> [run_args...]")
        sys.exit(1)

    action = sys.argv[1]

    # Set up the environment
    env = os.environ.copy()
    env["CMAKE_CXX_COMPILER"] = "clang++-16"
    env["CMAKE_C_COMPILER"] = "clang-16"
    env["CMAKE_CXX_FLAGS"] = "-stdlib=libc++"

    build_dir = "build"

    if action == "build":
        os.makedirs(build_dir, exist_ok=True)
        run_command(["cmake", "-S", ".", "-B", build_dir], env=env)
        run_command(["cmake", "--build", build_dir], env=env)
    elif action == "test":
        run_command(["ctest", "--test-dir", build_dir])
    elif action == "run":
        # The first argument to the executable is the executable name itself.
        # The remaining arguments are passed to the program.
        executable_path = os.path.join(build_dir, "chthollyc")
        run_args = [executable_path] + sys.argv[2:]
        run_command(run_args)
    else:
        print(f"Unknown action: {action}")
        sys.exit(1)

if __name__ == "__main__":
    main()
