import os
import subprocess
import argparse
import shutil

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Executing: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        exit(1)

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

def run_project():
    """Builds and runs the Chtholly project."""
    build_project()
    build_dir = 'build'
    executable_path = os.path.join(build_dir, 'src', 'chtholly')
    run_command([executable_path], '.')

def clean_project():
    """Cleans the build directory."""
    build_dir = 'build'
    if os.path.exists(build_dir):
        print(f"Removing directory: {build_dir}")
        shutil.rmtree(build_dir)

def main():
    parser = argparse.ArgumentParser(description="Chtholly build script")
    parser.add_argument('action', choices=['build', 'test', 'run', 'clean'], help="Action to perform")
    args = parser.parse_args()

    if args.action == 'build':
        build_project()
    elif args.action == 'test':
        test_project()
    elif args.action == 'run':
        run_project()
    elif args.action == 'clean':
        clean_project()

if __name__ == "__main__":
    main()
