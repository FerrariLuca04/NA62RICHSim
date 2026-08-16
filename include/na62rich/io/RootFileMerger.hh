#ifndef ROOT_FILE_MERGER_HH
#define ROOT_FILE_MERGER_HH

#include <filesystem>

bool MergeRootFiles();

std::filesystem::path makeUniquePath(const std::filesystem::path& path);

#endif