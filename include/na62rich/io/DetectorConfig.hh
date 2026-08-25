#ifndef NA62RICH_DETECTOR_CONFIG_HH
#define NA62RICH_DETECTOR_CONFIG_HH

#include "globals.hh"

#include <string>
#include <vector>
#include <limits>

struct WorldParams
{
    G4double length =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double height =
        std::numeric_limits<G4double>::quiet_NaN();

    std::string material;
};

struct GasParams
{
    G4double length =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double radius =
        std::numeric_limits<G4double>::quiet_NaN();

    std::string material;

    std::vector<G4double> photonEnergies;
    std::vector<G4double> refractiveIndex;
};

struct MirrorParams
{
    G4double curvatureRadius =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double outerRadius =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double innerRadius =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double thickness =
        std::numeric_limits<G4double>::quiet_NaN();

    G4double posZ =
        std::numeric_limits<G4double>::quiet_NaN();

    std::string material;

    std::vector<G4double> photonEnergies;
    std::vector<G4double> reflectivity;
};

struct PMTParams
{
    G4double PMTradius =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double diskRadius =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double thickness =
        std::numeric_limits<G4double>::quiet_NaN();

    G4double posR =
        std::numeric_limits<G4double>::quiet_NaN();
    G4double posZ =
        std::numeric_limits<G4double>::quiet_NaN();

    std::string material;

    std::vector<G4double> photonEnergies;
    std::vector<G4double> quantumEfficiency;
};

extern WorldParams* worldParams;
extern GasParams* gasParams;
extern MirrorParams* mirrorParams;
extern PMTParams* pmtParams;

void SetDetectorParams(const std::string& filename);

#endif