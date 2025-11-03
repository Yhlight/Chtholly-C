#pragma once

#include <string>

#include <vector>
#include <memory>
#include "AST/Stmt.h"
#include "Resolver.h"

class Chtholly {
public:
    Chtholly() = default;
    std::string run(const std::string& source);
    std::vector<std::unique_ptr<Stmt>> run(const std::string& source, bool is_module);

    static void runFile(const std::string& path);
    static void runPrompt();

private:
    Resolver resolver;
};
