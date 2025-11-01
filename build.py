import subprocess
import sys
import os

def run_command(command):
    """Executes a command and exits if it fails."""
    try:
        subprocess.run(command, check=True, shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e}")
        sys.exit(1)

def build():
    """Configures and builds the project using CMake."""
    if not os.path.exists("build"):
        os.makedirs("build")
    os.chdir("build")
    run_command("cmake ..")
    run_command("cmake --build .")
    os.chdir("..")

def run_tests():
    """Runs the tests."""
    if not os.path.exists("build/tests/chtholly_tests"):
        print("Tests not found. Building first.")
        build()
    run_command("./build/tests/chtholly_tests")


if __name__ == "__main__":
    if len(sys.argv) > 1:
        if sys.argv[1] == "build":
            build()
        elif sys.argv[1] == "test":
            run_tests()
        else:
            print(f"Unknown command: {sys.argv[1]}")
    else:
        print("Usage: python build.py [build|test]")
