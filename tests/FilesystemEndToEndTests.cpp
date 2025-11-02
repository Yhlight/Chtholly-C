#include "gtest/gtest.h"
#include "../src/Chtholly.h"
#include <fstream>
#include <string>
#include <cstdio> // For std::remove

TEST(FileSystemEndToEndTest, WriteAndReadFile) {
    // 1. Write a Chtholly program that uses fs_write.
    const std::string test_file_path = "test_output.txt";
    const std::string test_content = "Hello from Chtholly!";
    std::string chtholly_write_source = "fs_write(\"" + test_file_path + "\", \"" + test_content + "\");";

    // 2. Compile and run the Chtholly program.
    Chtholly chtholly;
    chtholly.run(chtholly_write_source, false);

    // 3. Verify that the file was created with the correct content.
    std::ifstream created_file(test_file_path);
    ASSERT_TRUE(created_file.is_open());
    std::string file_content((std::istreambuf_iterator<char>(created_file)),
                             std::istreambuf_iterator<char>());
    created_file.close();
    ASSERT_EQ(file_content, test_content);

    // 4. Write a Chtholly program that uses fs_read.
    const std::string read_output_path = "read_output.txt";
    std::string chtholly_read_source =
        "let content = fs_read(\"" + test_file_path + "\");\n"
        "fs_write(\"" + read_output_path + "\", content);";

    // 5. Compile and run the second Chtholly program.
    chtholly.run(chtholly_read_source, false);

    // 6. Verify that the second file was created with the correct content.
    std::ifstream read_file(read_output_path);
    ASSERT_TRUE(read_file.is_open());
    std::string read_content((std::istreambuf_iterator<char>(read_file)),
                           std::istreambuf_iterator<char>());
    read_file.close();
    ASSERT_EQ(read_content, test_content);

    // 7. Clean up the created files.
    std::remove(test_file_path.c_str());
    std::remove(read_output_path.c_str());
}
