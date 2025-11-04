import argparse
import os
import subprocess

def run_command(command, cwd=None):
    """Runs a command and prints its output."""
    try:
        result = subprocess.run(command, check=True, cwd=cwd, capture_output=True, text=True)
        print(result.stdout)
        if result.stderr:
            print(result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}")
        print(e.stdout)
        print(e.stderr)
        exit(1)

def build(args):
    """Builds the project using CMake."""
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    run_command(["cmake", "-S", ".", "-B", build_dir], cwd=None)
    run_command(["cmake", "--build", build_dir], cwd=None)

def run(args):
    """Runs the chtholly executable."""
    build(args)
    executable = os.path.join("build", "chtholly")
    if not os.path.exists(executable):
        print(f"Executable not found: {executable}")
        exit(1)

    command = [executable]
    if args.source:
        command.append(args.source)
    if args.output:
        command.append(args.output)

    run_command(command)


def test(args):
    """Runs the test suite."""
    build(args)
    run_command(["ctest"], cwd="build")


def main():
    parser = argparse.ArgumentParser(description="Chtholly build script")
    subparsers = parser.add_subparsers(dest="command", required=True)

    build_parser = subparsers.add_parser("build", help="Build the project")
    build_parser.set_defaults(func=build)

    run_parser = subparsers.add_parser("run", help="Run the chtholly executable")
    run_parser.add_argument("--source", help="Source file to compile", required=True)
    run_parser.add_argument("--output", help="Output file", required=True)
    run_parser.set_defaults(func=run)

    test_parser = subparsers.add_parser("test", help="Run the test suite")
    test_parser.set_defaults(func=test)

    args = parser.parse_args()
    args.func(args)

if __name__ == "__main__":
    main()
