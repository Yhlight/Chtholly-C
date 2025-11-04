#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_file> <output_file>" << std::endl;
        return 1;
    }

    std::cout << "Chtholly compiler" << std::endl;
    std::cout << "Source file: " << argv[1] << std::endl;
    std::cout << "Output file: " << argv[2] << std::endl;

    return 0;
}
