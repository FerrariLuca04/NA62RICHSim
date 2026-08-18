#ifndef NA62RICH_DETECTOR_CONFIG_HH
#define NA62RICH_DETECTOR_CONFIG_HH

#include "globals.hh"

#include <string>
#include <vector>

struct WorldParams
{
    G4double length;
    G4double height;

    std::string material;
};

struct GasParams
{
    G4double length;
    G4double radius;

    std::string material;

    std::vector<G4double> photonEnergies;
    std::vector<G4double> refractiveIndex;
};

struct MirrorParams
{
    G4double curvatureRadius;
    G4double outerRadius;
    G4double innerRadius;
    G4double thickness;

    G4double posZ;

    std::string material;

    std::vector<G4double> photonEnergies;
    std::vector<G4double> reflectivity;
};

struct PMTParams
{
    G4double PMTradius;
    G4double diskRadius;
    G4double thickness;

    G4double posR;
    G4double posZ;

    std::string material;

    std::vector<G4double> photonEnergies;
    std::vector<G4double> quantumEfficiency;
};

extern WorldParams* worldParams;
extern GasParams* gasParams;
extern MirrorParams* mirrorParams;
extern PMTParams* pmtParams;

void FillParams(const std::string& filename);

std::string Trim(const std::string& str);
G4double GetLengthUnit(const std::string& unit);
G4double GetEnergyUnit(const std::string& unit);
std::vector<G4double> ParseEnergyVector(
    const std::string& text,
    std::size_t lineNumber
);
std::vector<G4double> ParseDimensionlessVector(
    const std::string& text,
    std::size_t lineNumber
);
G4double ParseLength(
    const std::string& text,
    std::size_t lineNumber
);

#endif