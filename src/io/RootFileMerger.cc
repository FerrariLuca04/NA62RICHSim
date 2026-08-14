#include "na62rich/io/RootFileMerger.hh"

#include "na62rich/io/OutputPaths.hh"

#include "TFileMerger.h"

#include <iostream>

bool MergeRootFiles()
{
    TFileMerger merger;

    merger.OutputFile(outputFile.string().c_str());

    for (const auto& entry :
         std::filesystem::directory_iterator(tmpDirectory))
    {
        if (!entry.is_regular_file()) {
            continue;
        }

        const auto filename =
            entry.path().filename().string();

        if (filename.starts_with("photon_hits_t") &&
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

    return merger.Merge();
}