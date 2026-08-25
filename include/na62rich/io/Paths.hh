#ifndef PATHS_HH
#define PATHS_HH

#include <filesystem>

inline const std::filesystem::path outputDirectory =
    "output";

inline const std::filesystem::path tmpDirectory =
    outputDirectory / "tmp";

inline std::filesystem::path outputFile =
    outputDirectory / "data_na62rich_sim.root";

inline const std::string tmpFileName =
    "photon_hits";

inline const std::filesystem::path configDirectory =
    "config";

inline std::filesystem::path detectorConfigFile =
    configDirectory / "default_detector.conf";
    
inline std::filesystem::path generatorConfigFile =
    configDirectory / "default_generator.conf";

#endif