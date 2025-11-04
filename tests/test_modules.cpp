#include "gtest/gtest.h"
#include "../src/Lexer.h"
#include "../src/PrattParser.h"
#include "../src/Resolver.h"
#include "../src/Transpiler.h"
#include "../src/Error.h"
#include <fstream>

class ModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(ModuleTest, ParseImport) {
    std::string source = "import \"my_module.cns\";";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* importStmt = dynamic_cast<ImportStmt*>(stmts[0].get());
    ASSERT_NE(importStmt, nullptr);
    ASSERT_EQ(std::get<std::string>(importStmt->path.literal), "my_module.cns");
}

TEST_F(ModuleTest, ResolveMissingModule) {
    std::string source = "import \"missing_module.cns\";";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ModuleTest, TranspileImport) {
    std::string module_source = "func my_func() -> int { return 42; }";
    std::string main_source = "import \"my_module.cns\"; let x = my_func();";

    std::ofstream module_file("my_module.cns");
    module_file << module_source;
    module_file.close();

    Lexer lexer(main_source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string output = transpiler.transpile(stmts);

    std::string expected_module = "int my_func() {\n"
                                  "    return 42;\n"
                                  "}\n\n";
    std::string expected_main = "const auto x = my_func();";

    ASSERT_TRUE(output.find(expected_module) != std::string::npos);
    ASSERT_TRUE(output.find(expected_main) != std::string::npos);
}
