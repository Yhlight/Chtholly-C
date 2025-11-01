#include "TestHelpers.h"
#include "gtest/gtest.h"
#include "CodeGen.h"
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"
#include <string>

using namespace chtholly;

TEST(VariableTest, LetDeclarationWithTypeAnnotation) {
    std::string source = "let x: int = 10;";
    std::string expected = "const int x = 10;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(VariableTest, MutDeclarationWithTypeAnnotation) {
    std::string source = "mut y: string = \"hello\";";
    std::string expected = "#include <string> std::string y = \"hello\";";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(VariableTest, LetDeclarationWithTypeInference) {
    std::string source = "let z = true;";
    std::string expected = "const auto z = true;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(VariableTest, MutDeclarationWithTypeInference) {
    std::string source = "mut a = 123;";
    std::string expected = "auto a = 123;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}
