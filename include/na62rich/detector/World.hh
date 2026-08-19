#ifndef NA62RICH_WORLD_HH
#define NA62RICH_WORLD_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

struct WorldParams;

class World : public CustomDetector {
    public:
        World(WorldParams* worldParams);

    protected:
        G4Material* CreateMaterial() override;
        G4VSolid* CreateSolid(std::string name) override;

        std::vector<G4VPhysicalVolume*> Place(
            G4LogicalVolume* mother,
            G4LogicalVolume* logical,
            std::string name
        ) override;
    
    private:
        WorldParams* fWorldParams;
};

#endif