#ifndef NA62RICH_TEST_DETECTOR_CONFIG_HH
#define NA62RICH_TEST_DETECTOR_CONFIG_HH

struct WorldParams;
struct GasParams;
struct MirrorParams;
struct PMTParams;

// Test function for config parameters
void TestDetectorConfig(
    const WorldParams& world,
    const GasParams& gas,
    const MirrorParams& mirror,
    const PMTParams& pmt
);

#endif