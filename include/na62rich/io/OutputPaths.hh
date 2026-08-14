#ifndef OUTPUT_PATHS_HH
#define OUTPUT_PATHS_HH

#include <filesystem>

inline const std::filesystem::path outputDirectory =
    "output";

inline const std::filesystem::path tmpDirectory =
    outputDirectory / "tmp";

inline const std::filesystem::path outputFile =
    outputDirectory / "data_na62rich_sim.root";

inline const std::filesystem::path tmpFile =
    tmpDirectory / "photon_hits.root";

#endif