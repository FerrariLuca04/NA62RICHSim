#ifndef NA62RICH_DETECTOR_CONSTRUCTION_HH
#define NA62RICH_DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class World;
class GasDetector;
class MirrorDetector;
class PMTdetector;

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        DetectorConstruction() = default;
        ~DetectorConstruction() override = default;

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField();

    private:
        World* fWorld = nullptr;
        GasDetector* fGas = nullptr;
        MirrorDetector* fMirror = nullptr;
        PMTdetector* fPMT = nullptr;
};

#endif