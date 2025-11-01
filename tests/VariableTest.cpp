#include "TestHelpers.h"
#include "gtest/gtest.h"
#include "CodeGen.h"
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"
#include <string>

using namespace chtholly;

const std::string PREAMBLE = "#include <string>\n#include <vector>\n#include <array>\n";

TEST(VariableTest, LetDeclarationWithTypeAnnotation) {
    std::string source = "let x: int = 10;";
    std::string expected = PREAMBLE + "const int x = 10;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(VariableTest, MutDeclarationWithTypeAnnotation) {
    std::string source = "mut y: string = \"hello\";";
    std::string expected = PREAMBLE + "std::string y = \"hello\";";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(VariableTest, LetDeclarationWithTypeInference) {
    std::string source = "let z = true;";
    std::string expected = PREAMBLE + "const bool z = true;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(VariableTest, MutDeclarationWithTypeInference) {
    std::string source = "mut a = 123;";
    std::string expected = PREAMBLE + "int a = 123;";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}
