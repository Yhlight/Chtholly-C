import os
import subprocess
import sys

def run_command(args, cwd=None):
    """Runs a command and exits if it fails."""
    try:
        subprocess.run(args, check=True, cwd=cwd)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {' '.join(args)}", file=sys.stderr)
        sys.exit(e.returncode)
    except FileNotFoundError:
        print(f"Error: Command '{args[0]}' not found. Make sure it's in your PATH.", file=sys.stderr)
        sys.exit(1)

def main():
    """Builds and optionally tests the Chtholly project."""
    build_dir = "build"
    os.makedirs(build_dir, exist_ok=True)

    # Configure the project
    run_command(["cmake", ".."], cwd=build_dir)

    # Build the project
    run_command(["cmake", "--build", "."], cwd=build_dir)

    # Run tests if requested
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        print("\nRunning tests...")
        run_command(["ctest", "--output-on-failure"], cwd=build_dir)

if __name__ == "__main__":
    main()
