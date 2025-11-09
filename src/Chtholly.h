#ifndef CHTHOLLY_CHTHOLLY_H
#define CHTHOLLY_CHTHOLLY_H

#include <string>

namespace chtholly {
    class Chtholly {
    public:
        void runFile(const std::string &path);
        void runPrompt();

    private:
        void run(const std::string &source);
    };
}

#endif //CHTHOLLY_CHTHOLLY_H
