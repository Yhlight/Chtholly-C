#include "Filesystem.h"
#include <filesystem>

namespace chtholly {

bool Filesystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool Filesystem::is_directory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

} // namespace chtholly
