#ifndef NA62RICH_PRIMARY_GENERATOR_CONFIG_HH
#define NA62RICH_PRIMARY_GENERATOR_CONFIG_HH

#include "globals.hh"

#include <string>
#include <vector>
#include <limits>

class TFile;

struct EntranceParams {
    G4double rMin =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double rMax =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double phiMin =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double phiMax =
        std::numeric_limits<G4double>::quiet_NaN();
};

struct DecayRegionParams {
    G4double start =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double length =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double sigmaX =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double sigmaY =
        std::numeric_limits<G4double>::quiet_NaN();
};

struct ParticleParams {
    G4double pMin =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double pMax =
        std::numeric_limits<G4double>::quiet_NaN();
    std::string type;
};

extern EntranceParams* entranceParams;
extern DecayRegionParams* decayRegionParams;
extern ParticleParams* particleParams;

void SetGeneratorParams(const std::string& filename);

void WriteGeneratorConfig(TFile& file);

#endif