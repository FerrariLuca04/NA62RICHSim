#ifndef NA62RICH_PMT_DETECTOR_HH
#define NA62RICH_PMT_DETECTOR_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

class G4OpticalSurface;

class PMTdetector : public CustomDetector {
    public:
        PMTdetector(
            G4double diskRadius,
            G4double PMTradius,
            G4double thickness, 
            G4double posZ,
            G4double posR
        );

    protected:
        G4Material* CreateMaterial() override;
        G4VSolid* CreateSolid(std::string name) override;

        std::vector<G4VPhysicalVolume*> Place(
            G4LogicalVolume* mother,
            G4LogicalVolume* logical,
            std::string name
        ) override;
    
    private:
        G4double fDiskRadius;
        G4double fPMTradius;
        G4double fThickness;

        G4double fPosZ;
        G4double fPosR;
};

bool IsHexagonInsideCircle(
    G4double x,
    G4double y,
    G4double hexRadius,
    G4double circleRadius
);

#endif