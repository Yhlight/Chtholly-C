#ifndef CHTHOLLY_FILESYSTEM_H
#define CHTHOLLY_FILESYSTEM_H

#include <string>

namespace chtholly {

class Filesystem {
public:
    // Path functions
    static bool exists(const std::string& path);
    static bool is_directory(const std::string& path);
};

} // namespace chtholly

#endif // CHTHOLLY_FILESYSTEM_H
