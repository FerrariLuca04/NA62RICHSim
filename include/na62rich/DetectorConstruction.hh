#ifndef NA62RICH_DETECTOR_CONSTRUCTION_HH
#define NA62RICH_DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
};

#endif