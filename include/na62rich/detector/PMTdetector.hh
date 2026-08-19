#ifndef NA62RICH_PMT_DETECTOR_HH
#define NA62RICH_PMT_DETECTOR_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

struct PMTParams;
class G4OpticalSurface;

class PMTdetector : public CustomDetector {
    public:
        PMTdetector(PMTParams* pmtParams);

    protected:
        G4Material* CreateMaterial() override;
        G4VSolid* CreateSolid(std::string name) override;

        std::vector<G4VPhysicalVolume*> Place(
            G4LogicalVolume* mother,
            G4LogicalVolume* logical,
            std::string name
        ) override;
    
    private:
        PMTParams* fPMTParams;
};

bool IsHexagonInsideCircle(
    G4double x,
    G4double y,
    G4double hexRadius,
    G4double circleRadius
);

#endif