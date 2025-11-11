#include <gtest/gtest.h>
#include "TestHelpers.h"
#include <fstream>
#include <filesystem>

class FilesystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::create_directory("test_dir");
        std::ofstream("test_dir/file1.txt") << "hello";
        std::ofstream("test_dir/file2.txt") << "world";
        std::filesystem::create_directory("test_dir/nested_dir");
        std::ofstream("test_file.txt") << "test content";
    }

    void TearDown() override {
        std::filesystem::remove_all("test_dir");
        std::filesystem::remove("test_file.txt");
    }
};

TEST_F(FilesystemTest, ReadFile) {
    std::string source = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            let content = fs_read("test_file.txt");
            print(content);
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "test content\n");
}

TEST_F(FilesystemTest, WriteFile) {
    std::string source = R"(
        import filesystem;
        func main() -> int {
            fs_write("new_file.txt", "new content");
            return 0;
        }
    )";
    chtholly::run(source, true);

    std::ifstream file("new_file.txt");
    std::stringstream buffer;
    buffer << file.rdbuf();
    ASSERT_EQ(buffer.str(), "new content");
    std::filesystem::remove("new_file.txt");
}

TEST_F(FilesystemTest, Exists) {
    std::string source = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            print(fs_exists("test_dir"));
            print(fs_exists("test_file.txt"));
            print(fs_exists("non_existent_file.txt"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\ntrue\nfalse\n");
}

TEST_F(FilesystemTest, IsFile) {
    std::string source = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            print(fs_is_file("test_file.txt"));
            print(fs_is_file("test_dir"));
            print(fs_is_file("non_existent_file.txt"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\nfalse\n");
}

TEST_F(FilesystemTest, IsDir) {
    std::string source = R"(
        import filesystem;
        import iostream;
        func main() -> int {
            print(fs_is_dir("test_dir"));
            print(fs_is_dir("test_file.txt"));
            print(fs_is_dir("non_existent_dir"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\nfalse\n");
}

TEST_F(FilesystemTest, ListDir) {
    std::string source = R"(
        import filesystem;
        import iostream;
        import array;
        func main() -> int {
            mut entries = fs_list_dir("test_dir");
            entries.sort();
            for (let entry in entries) {
                print(entry);
            }
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);

    std::string expected_output = "test_dir/file1.txt\ntest_dir/file2.txt\ntest_dir/nested_dir\n";
    std::string actual_output = result.stdout_output;

    // Normalize paths for comparison
    std::replace(expected_output.begin(), expected_output.end(), '/', std::filesystem::path::preferred_separator);

    // The order of directory entries is not guaranteed, so we'll check for presence.
    ASSERT_NE(actual_output.find("test_dir/file1.txt"), std::string::npos);
    ASSERT_NE(actual_output.find("test_dir/file2.txt"), std::string::npos);
    ASSERT_NE(actual_output.find("test_dir/nested_dir"), std::string::npos);
}

TEST_F(FilesystemTest, RemoveFile) {
    std::ofstream("file_to_remove.txt");
    ASSERT_TRUE(std::filesystem::exists("file_to_remove.txt"));
    std::string source = R"(
        import filesystem;
        func main() -> int {
            fs_remove("file_to_remove.txt");
            return 0;
        }
    )";
    chtholly::run(source, true);
    ASSERT_FALSE(std::filesystem::exists("file_to_remove.txt"));
}

TEST_F(FilesystemTest, RemoveDirectory) {
    std::filesystem::create_directory("dir_to_remove");
    std::ofstream("dir_to_remove/file.txt");
    ASSERT_TRUE(std::filesystem::exists("dir_to_remove"));
    std::string source = R"(
        import filesystem;
        func main() -> int {
            fs_remove("dir_to_remove");
            return 0;
        }
    )";
    chtholly::run(source, true);
    ASSERT_FALSE(std::filesystem::exists("dir_to_remove"));
}
