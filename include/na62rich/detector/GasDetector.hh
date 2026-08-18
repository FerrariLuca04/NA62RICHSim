#ifndef NA62RICH_GAS_VOLUME_HH
#define NA62RICH_GAS_VOLUME_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

struct GasParams;

class G4Material;
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

class GasDetector : public CustomDetector {
    public:
        GasDetector(GasParams* gasParams);

    protected:
        G4Material* CreateMaterial() override;
        G4VSolid* CreateSolid(std::string name) override;

        std::vector<G4VPhysicalVolume*> Place(
            G4LogicalVolume* mother,
            G4LogicalVolume* logical,
            std::string name
        ) override;
    
    private:
        GasParams* fGasParams;
};

#endif