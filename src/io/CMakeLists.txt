# src/io/CMakeLists.txt

target_sources(
    na62_rich_sim
    PRIVATE
        RootFileMerger.cc
        DetectorConfig.cc
        TestDetectorConfig.cc
        ParsersConfig.cc
        PrimaryGeneratorConfig.cc
)