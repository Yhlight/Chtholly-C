import os
import subprocess
import argparse
import glob

def run_command(command):
    """Runs a command and checks for errors."""
    try:
        # The timeout is set to 10 seconds to prevent infinite loops in the compiled code
        result = subprocess.run(command, check=True, shell=True, capture_output=True, text=True, timeout=60)
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {command}")
        print(f"Stderr: {e.stderr}")
        exit(1)
    except subprocess.TimeoutExpired as e:
        print(f"Command timed out: {command}")
        exit(1)


def main():
    parser = argparse.ArgumentParser(description="Build and run the Chtholly compiler.")
    parser.add_argument("--run", action="store_true", help="Run the compiler after building.")
    parser.add_argument("--test", action="store_true", help="Run tests after building.")
    parser.add_argument("--source", type=str, help="Source file to compile.")
    parser.add_argument("--output", type=str, help="Output file for the compiled code.")
    args = parser.parse_args()

    # Create a build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Configure the project with CMake
    print("Configuring with CMake...")
    run_command("cmake -S . -B build")

    # Build the project
    print("Building project...")
    run_command("cmake --build build")

    if args.run:
        if not args.source or not args.output:
            print("Error: --source and --output are required when using --run.")
            exit(1)

        compiler_path = os.path.join("build", "chtholly")
        print(f"Running compiler on {args.source}...")
        run_command(f"{compiler_path} {args.source} {args.output}")
        print(f"Successfully compiled {args.source} to {args.output}")


    if args.test:
        print("Running tests...")
        compiler_path = os.path.join("build", "chtholly")
        test_files = glob.glob("tests/*.cns")

        for test_file in test_files:
            print(f"--- Running test: {test_file} ---")

            # Parse expected output from comments
            expected_output = []
            with open(test_file, 'r') as f:
                for line in f:
                    if '// expect:' in line:
                        expected = line.split('// expect:')[1].strip()
                        expected_output.append(expected)
            expected_output_str = "\n".join(expected_output) + "\n"

            output_cpp = os.path.splitext(test_file)[0] + ".cpp"
            output_exe = os.path.splitext(test_file)[0]

            # Compile Chtholly to C++
            run_command(f"{compiler_path} {test_file} {output_cpp}")

            # Compile C++ to executable
            run_command(f"g++ {output_cpp} -o {output_exe}")

            # Run the executable
            result = run_command(f"./{output_exe}")
            actual_output = result.stdout

            # Compare actual output with expected output
            if actual_output != expected_output_str:
                print("Test Failed!")
                print(f"Expected output:\n---\n{expected_output_str}---")
                print(f"Actual output:\n---\n{actual_output}---")
                print(f"Intermediate C++ code saved at: {output_cpp}")
                # Clean up generated files before exiting
                os.remove(output_exe)
                exit(1)
            else:
                print("Test Passed!")

            # Clean up generated files
            os.remove(output_cpp)
            os.remove(output_exe)

        print("All tests ran successfully!")


if __name__ == "__main__":
    main()
