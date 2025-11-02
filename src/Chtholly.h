#pragma once

#include <string>

#include <vector>
#include <memory>
#include "AST/Stmt.h"
#include "Resolver.h"

class Chtholly {
public:
    Chtholly() = default;
    std::vector<std::unique_ptr<Stmt>> run(const std::string& source);
    Resolver& get_resolver() { return resolver; }

    static void runFile(const std::string& path);
    static void runPrompt();

private:
    Resolver resolver;
};
