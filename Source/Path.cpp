#include <Path.h>

#include <CMakeProjectMacros.h>


std::string GetFullPath(const std::string& RelativePath)
{
    return std::string(PROJECT_DIR) + RelativePath;
}