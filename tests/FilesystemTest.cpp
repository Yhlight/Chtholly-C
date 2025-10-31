#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Sema.h"
#include "CodeGen.h"

#include <string>
#include <vector>

using namespace chtholly;

TEST(FilesystemTest, PathJoin) {
    std::string source = R"(
        import filesystem;
        let a = "path";
        let b = "to";
        let c = path::join(a, b);
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();

    Sema sema;
    sema.analyze(*ast);

    CodeGen codegen(sema);
    std::string cppCode = codegen.generate(*ast);

    std::string expectedCode =
        "#include <string>\n"
        "#include <vector>\n"
        "const auto a = \"path\";\n"
        "const auto b = \"to\";\n"
        "const auto c = (a + \"/\" + b);\n";

    ASSERT_EQ(cppCode, expectedCode);
}
