#ifndef NA62RICH_PRIMARY_GENERATOR_CONFIG_HH
#define NA62RICH_PRIMARY_GENERATOR_CONFIG_HH

#include "globals.hh"

#include <string>
#include <vector>

struct EntranceParams {
    G4double rMin;
    G4double rMax;
    G4double phiMin;
    G4double phiMax;
};

struct DecayRegionParams {
    G4double start;
    G4double length;
    G4double sigmaX;
    G4double sigmaY;
};

void SetGeneratorParams(const std::string& filename);

extern EntranceParams* entranceParams;
extern DecayRegionParams* decayRegionParams;

#endif