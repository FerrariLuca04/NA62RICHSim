#ifndef NA62RICH_WORLD_HH
#define NA62RICH_WORLD_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

class World : public CustomDetector {
    public:
        World(
            G4double dimX,
            G4double dimY,
            G4double dimZ
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
        G4double fDimX;
        G4double fDimY;
        G4double fDimZ;
};

#endif