#ifndef ROOT_FILE_MERGER_HH
#define ROOT_FILE_MERGER_HH

#include <filesystem>

std::filesystem::path MergeRootFiles();

std::filesystem::path makeUniquePath(const std::filesystem::path& path);

#endif