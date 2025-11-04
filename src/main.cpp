#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_file> <output_file>" << std::endl;
        return 1;
    }
    std::cout << "Chtholly compiler placeholder" << std::endl;
    return 0;
}
