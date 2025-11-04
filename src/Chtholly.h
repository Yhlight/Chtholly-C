#pragma once

#include <string>
#include <vector>
#include <memory>
#include "AST/Stmt.h"

class Chtholly {
public:
    Chtholly();
    std::vector<std::unique_ptr<Stmt>> run(const std::string& source, bool is_module = false);

    static void runFile(const std::string& path);
    static void runPrompt();
};
