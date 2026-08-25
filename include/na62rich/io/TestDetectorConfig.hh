#ifndef NA62RICH_TEST_DETECTOR_CONFIG_HH
#define NA62RICH_TEST_DETECTOR_CONFIG_HH

struct WorldParams;
struct GasParams;
struct MirrorParams;
struct PMTParams;

struct EntranceParams;
struct DecayRegionParams;
struct ParticleParams;

// Test function for config parameters
void TestDetectorConfig(
    const WorldParams& world,
    const GasParams& gas,
    const MirrorParams& mirror,
    const PMTParams& pmt
);

void TestPrimaryGeneratorConfig(
    const EntranceParams& entrance,
    const DecayRegionParams& decayRegion,
    const ParticleParams& particle
);

#endif