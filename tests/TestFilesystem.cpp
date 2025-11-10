#include <gtest/gtest.h>
#include "TestHelpers.h"
#include <fstream>
#include <filesystem>

class FilesystemTest : public TranspilerTest {
protected:
    void SetUp() override {
        std::filesystem::create_directory("test_dir");
        std::ofstream("test_dir/file1.txt") << "content1";
        std::ofstream("test_dir/file2.txt") << "content2";
    }

    void TearDown() override {
        std::filesystem::remove_all("test_dir");
    }
};

TEST_F(FilesystemTest, ReadAndWrite) {
    std::string code = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            let content = filesystem::read("test_dir/file1.txt");
            print(content);
            filesystem::write("test_dir/file3.txt", "new_content");
            let new_content = filesystem::read("test_dir/file3.txt");
            print(new_content);
            return 0;
        }
    )";
    RunResult result = run_and_capture(code);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "content1\nnew_content\n");
}

TEST_F(FilesystemTest, ListDir) {
    std::string code = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            let entries = filesystem::list_dir("test_dir");
            for (let entry in entries) {
                print(entry);
            }
            return 0;
        }
    )";
    RunResult result = run_and_capture(code);
    ASSERT_EQ(result.exit_code, 0);
    // The order is not guaranteed, so we check for the presence of each file.
    ASSERT_NE(result.stdout_output.find("test_dir/file1.txt"), std::string::npos);
    ASSERT_NE(result.stdout_output.find("test_dir/file2.txt"), std::string::npos);
}

TEST_F(FilesystemTest, IsFileAndIsDir) {
    std::string code = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            print(filesystem::is_file("test_dir/file1.txt"));
            print(filesystem::is_dir("test_dir"));
            print(filesystem::is_file("test_dir"));
            print(filesystem::is_dir("test_dir/file1.txt"));
            return 0;
        }
    )";
    RunResult result = run_and_capture(code);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\ntrue\nfalse\nfalse\n");
}

TEST_F(FilesystemTest, Remove) {
    std::string code = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            filesystem::remove("test_dir/file1.txt");
            print(filesystem::is_file("test_dir/file1.txt"));
            return 0;
        }
    )";
    RunResult result = run_and_capture(code);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "false\n");
}
