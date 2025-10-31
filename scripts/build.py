import os
import subprocess
import sys

def run_command(command):
    """Runs a command and exits if it fails."""
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}", file=sys.stderr)
        sys.exit(e.returncode)

def main():
    """Configures and builds the project using CMake."""
    if not os.path.exists("build"):
        os.makedirs("build")

    os.chdir("build")

    run_command(["cmake", ".."])
    run_command(["cmake", "--build", "."])

if __name__ == "__main__":
    main()
