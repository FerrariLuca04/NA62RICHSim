#include "na62rich/io/RootFileMerger.hh"

#include "na62rich/io/Paths.hh"

#include "TFileMerger.h"

#include <iostream>

std::filesystem::path MergeRootFiles()
{
    TFileMerger merger;

    auto newOutputFile = makeUniquePath(outputFile);

    merger.OutputFile(newOutputFile.string().c_str());

    for (const auto& entry :
         std::filesystem::directory_iterator(tmpDirectory))
    {
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto filename =
            entry.path().filename().string();

        if (filename.starts_with("photon_hits") &&
            filename.ends_with(".root"))
        {
            std::cout
                << "Adding " << filename
                << '\n';

            merger.AddFile(
                entry.path().string().c_str()
            );
        }
    }

    if (!merger.Merge()) {
        throw std::runtime_error(
            "Failed to merge ROOT files."
        );
    }

    return newOutputFile;
}

std::filesystem::path makeUniquePath(const std::filesystem::path& path) 
{
    if (!std::filesystem::exists(path)) {
        return path;
    }

    const auto parent = path.parent_path();
    const auto stem = path.stem().string();
    const auto extension = path.extension().string();

    int copyNumber = 1;

    std::filesystem::path newPath;

    do {
        newPath =
            parent /
            (stem + "_" +
             std::to_string(copyNumber) +
             extension);

        ++copyNumber;

    } while (std::filesystem::exists(newPath));

    return newPath;
}