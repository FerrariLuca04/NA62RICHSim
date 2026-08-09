#ifndef NA62RICH_DETECTOR_CONSTRUCTION_HH
#define NA62RICH_DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4Material;
class G4LogicalVolume;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        DetectorConstruction() = default;
        ~DetectorConstruction() override = default;

        G4VPhysicalVolume* Construct() override;

    private:
        G4VPhysicalVolume* fWorldPhysical = nullptr;

        // Materials
        void DefineMaterials();

        G4Material* fWorldMaterial = nullptr;
        G4Material* fGasMaterial = nullptr;
        G4Material* fMirrorMaterial = nullptr;

        // Shared volumes
        G4VPhysicalVolume* fGasPhysical;

        // Geometry builders
        G4LogicalVolume* BuildWorld();
        G4LogicalVolume* BuildGas(G4LogicalVolume* mother);

        void BuildMirror(G4LogicalVolume* mother);
};

#endif