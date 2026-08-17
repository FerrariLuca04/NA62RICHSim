#ifndef NA62RICH_MIRROR_DETECTOR_HH
#define NA62RICH_MIRROR_DETECTOR_HH

#include "na62rich/detector/CustomDetector.hh"

#include "globals.hh"

#include <string>
#include <vector>

class G4OpticalSurface;

class MirrorDetector : public CustomDetector {
    public:
        MirrorDetector(
            G4double curvatureRadius,
            G4double innerRadius,
            G4double outerRadius,
            G4double thickness, 
            G4double posZ
        );

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
        G4double fCurvatureRadius;
        G4double fInnerRadius;
        G4double fOuterRadius;
        G4double fThickness;

        G4double fThetaMax;

        G4double fPosZ;

        G4OpticalSurface* fSurface = nullptr;
};

#endif