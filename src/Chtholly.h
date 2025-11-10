#ifndef CHTHOLLY_CHTHOLLY_H
#define CHTHOLLY_CHTHOLLY_H

#include <string>
#include <vector>

namespace chtholly {
    class Chtholly {
    public:
        int runFile(const std::string &path);
        void runPrompt();
        int run(const std::vector<std::string>& files, const std::string& output_file, const std::string& cxx_compiler);

    private:
        int run(const std::string &source);
    };
}

#endif //CHTHOLLY_CHTHOLLY_H
