#ifndef NA62RICH_MIRROR_DETECTOR_HH
#define NA62RICH_MIRROR_DETECTOR_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

struct MirrorParams;
class G4OpticalSurface;

class MirrorDetector : public CustomDetector {
    public:
        MirrorDetector(MirrorParams* mirrorParams);

        void SetSurface(
            G4VPhysicalVolume* physical1,
            G4VPhysicalVolume* physical2
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
        MirrorParams* fMirrorParams;

        G4OpticalSurface* fSurface = nullptr;
};

#endif