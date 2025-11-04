import os
import subprocess
import sys

def main():
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure the project
    subprocess.run(["cmake", "-S", ".", "-B", build_dir], check=True)

    # Build the project
    subprocess.run(["cmake", "--build", build_dir], check=True)

    # Run tests if requested
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        ctest_command = ["ctest", "--test-dir", build_dir]
        subprocess.run(ctest_command, check=True)

if __name__ == "__main__":
    main()
