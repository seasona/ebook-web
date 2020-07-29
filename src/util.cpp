#include "util.h"

namespace Util{

std::string normalize(const std::string& path) {
    std::string Normalized = path;

#ifdef WIN32
    // converts all '\' to '/' (only on WIN32)
    size_t i, imax;

    for (i = 0, imax = Normalized.length(); i < imax; i++)
        if (Normalized[i] == '\\') Normalized[i] = '/';

#endif
    // Collapse '//' to '/'
    std::string::size_type pos = 1;

    while (true) {
        pos = Normalized.find("//", pos);

        if (pos == std::string::npos) break;

        Normalized.erase(pos, 1);
    }

     // Remove leading './'
    while (!Normalized.compare(0, 2, "./")) Normalized = Normalized.substr(2);

    // Collapse '/./' to '/'
    pos = 0;

    while (true) {
        pos = Normalized.find("/./", pos);

        if (pos == std::string::npos) break;

        Normalized.erase(pos, 2);
    }

    // Collapse '[^/]+/../' to '/'
    std::string::size_type start = Normalized.length();

    while (true) {
        pos = Normalized.rfind("/../", start);

        if (pos == std::string::npos) break;

        start = Normalized.rfind('/', pos - 1);

        if (start == std::string::npos) break;

        if (!Normalized.compare(start, 4, "/../")) continue;

        Normalized.erase(start, pos - start + 3);
        start = Normalized.length();
    }

    return Normalized;
}

};