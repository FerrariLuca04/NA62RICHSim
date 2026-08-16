# src/detector/CMakeLists.txt

target_sources(
    na62_rich_sim
    PRIVATE
        DetectorConstruction.cc
        PhotonSensitiveDetector.cc
        CustomDetector.cc
)