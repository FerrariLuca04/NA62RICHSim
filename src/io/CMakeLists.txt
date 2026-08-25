# src/io/CMakeLists.txt

target_sources(
    na62_rich_sim
    PRIVATE
        RootFileMerger.cc
        DetectorConfig.cc
        TestConfig.cc
        ParsersConfig.cc
        PrimaryGeneratorConfig.cc
)