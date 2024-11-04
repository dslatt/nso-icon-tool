#pragma once

#include <functional>
#include <string>

namespace extract {
void extract(const std::string& filename, const std::string& workingPath, bool overwriteExisting);
}