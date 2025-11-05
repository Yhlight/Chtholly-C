import os
import subprocess
import argparse
import shutil
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Executing: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)
    return result

def build_project():
    """Builds the Chtholly project."""
    build_dir = 'build'
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    run_command(['cmake', '..'], build_dir)
    run_command(['cmake', '--build', '.'], build_dir)

def test_project():
    """Tests the Chtholly project."""
    build_project()
    build_dir = 'build'
    run_command(['ctest', '--output-on-failure'], build_dir)

def run_project(args):
    """Builds and runs the Chtholly project."""
    build_project()
    build_dir = 'build'

    # Find the executable path using CMake
    result = run_command(['cmake', '--find-package', '-DNAME=chtholly', '-DCOMPILER_ID=GNU', '-DLANG=CXX', '-DMODE=EXIST'], build_dir)
    executable_path = ""
    for line in result.stdout.splitlines():
        if "chtholly" in line and "chtholly-targets" in line:
            # This is a bit of a hack, but it's more robust than a hardcoded path.
            # We're looking for a line like:
            # chtholly_DIR set to /path/to/build/src/chtholly-targets
            # and then we construct the path to the executable from that.
            parts = line.split(" ")
            if len(parts) > 3:
                chtholly_dir = parts[3]
                executable_path = os.path.join(chtholly_dir, "..", "chtholly")
                break

    if not executable_path or not os.path.exists(executable_path):
        # Fallback for other generators, though this is less robust
        executable_path = os.path.join(build_dir, 'src', 'chtholly')
        if not os.path.exists(executable_path):
            print("Could not find chtholly executable.")
            sys.exit(1)

    run_command([executable_path] + args, '.')

def clean_project():
    """Cleans the build directory."""
    build_dir = 'build'
    if os.path.exists(build_dir):
        print(f"Removing directory: {build_dir}")
        shutil.rmtree(build_dir)

def main():
    parser = argparse.ArgumentParser(description="Chtholly build script")
    subparsers = parser.add_subparsers(dest='action', required=True)

    subparsers.add_parser('build', help="Build the project")
    subparsers.add_parser('test', help="Build and run tests")
    run_parser = subparsers.add_parser('run', help="Build and run the project")
    run_parser.add_argument('args', nargs=argparse.REMAINDER, help="Arguments to pass to the executable")
    subparsers.add_parser('clean', help="Clean the build directory")

    args = parser.parse_args()

    if args.action == 'build':
        build_project()
    elif args.action == 'test':
        test_project()
    elif args.action == 'run':
        run_project(args.args)
    elif args.action == 'clean':
        clean_project()

if __name__ == "__main__":
    main()
