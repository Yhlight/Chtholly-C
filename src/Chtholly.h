#ifndef CHTHOLLY_CHTHOLLY_H
#define CHTHOLLY_CHTHOLLY_H

#include <string>

namespace chtholly {
    class Chtholly {
    public:
        Chtholly();
        explicit Chtholly(std::string cxx_compiler);
        int runFile(const std::string &path);
        void runPrompt();

    private:
        int run(const std::string &source);
        std::string cxx_compiler;
    };
}

#endif //CHTHOLLY_CHTHOLLY_H
