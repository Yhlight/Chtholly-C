#include <gtest/gtest.h>
#include "../src/common/Error.h"
#include "../src/frontend/Lexer.h"
#include "../src/frontend/Parser.h"
#include "../src/middle/Resolver.h"

class ErrorReportingTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(ErrorReportingTest, UnexpectedCharacter) {
    std::string source = "let x = @;";

    testing::internal::CaptureStderr();
    Lexer lexer(source);
    lexer.scanTokens();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(ErrorReporter::hadError);
    EXPECT_EQ(output, "[line 1] Error: Unexpected character.\n");
}

TEST_F(ErrorReportingTest, UnterminatedString) {
    std::string source = "let x = \"hello;";

    testing::internal::CaptureStderr();
    Lexer lexer(source);
    lexer.scanTokens();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(ErrorReporter::hadError);
    EXPECT_EQ(output, "[line 1] Error: Unterminated string.\n");
}

TEST_F(ErrorReportingTest, MissingSemicolon) {
    std::string source = "let x = 5";

    testing::internal::CaptureStderr();
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    parser.parse();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(ErrorReporter::hadError);
    EXPECT_EQ(output, "[line 1] Error at end: Expect ';' after variable declaration.\n");
}

TEST_F(ErrorReportingTest, InvalidAssignmentTarget) {
    std::string source = "5 = x;";

    testing::internal::CaptureStderr();
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    parser.parse();
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(ErrorReporter::hadError);
    EXPECT_EQ(output, "[line 1] Error at '=': Invalid assignment target.\n");
}
